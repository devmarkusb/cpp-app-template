#include "game_widget.hpp"

#include <QKeyEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPaintEvent>
#include <QRandomGenerator>
#include <QTimer>

#include <algorithm>
#include <cmath>

using mb::cpp_app_template::Direction;
using mb::cpp_app_template::SnakeGame;
using mb::cpp_app_template::Tile;

namespace {
constexpr int k_grid_w = 32;
constexpr int k_grid_h = 16;
} // namespace

GameWidget::GameWidget(QWidget* parent)
    : QWidget(parent)
    , game_(k_grid_w, k_grid_h, QRandomGenerator::global()->generate()) {
    setFocusPolicy(Qt::StrongFocus);
    setMinimumSize(640, 420);

    game_timer_ = new QTimer(this);
    connect(game_timer_, &QTimer::timeout, this, &GameWidget::onGameTick);
    game_timer_->start(tickIntervalMs());

    anim_timer_ = new QTimer(this);
    connect(anim_timer_, &QTimer::timeout, this, &GameWidget::onAnimTick);
    anim_timer_->start(16);
}

void GameWidget::togglePause() {
    if (game_.game_over()) {
        return;
    }
    paused_ = !paused_;
    emit pausedChanged(paused_);
    update();
}

void GameWidget::restartGame() {
    game_ = SnakeGame(k_grid_w, k_grid_h, QRandomGenerator::global()->generate());
    paused_ = false;
    game_timer_->setInterval(tickIntervalMs());
    game_timer_->start();
    emit scoreChanged(game_.score());
    emit gameEnded(false);
    emit pausedChanged(false);
    update();
}

QRectF GameWidget::fieldRect() const {
    const qreal m = 20.0;
    const qreal fw = width() - 2.0 * m;
    const qreal fh = height() - 2.0 * m;
    const qreal cell = std::floor(std::min(fw / k_grid_w, fh / k_grid_h));
    const qreal gw = cell * k_grid_w;
    const qreal gh = cell * k_grid_h;
    const qreal x = (width() - gw) / 2.0;
    const qreal y = (height() - gh) / 2.0;
    return {x, y, gw, gh};
}

QRectF GameWidget::cellRect(int gx, int gy) const {
    const QRectF fr = fieldRect();
    const qreal cw = fr.width() / k_grid_w;
    const qreal ch = fr.height() / k_grid_h;
    return {fr.x() + gx * cw, fr.y() + gy * ch, cw, ch};
}

int GameWidget::tickIntervalMs() const {
    const int score = game_.score();
    const int base = 130;
    const int bonus = std::min(85, score / 2);
    return std::max(32, base - bonus);
}

void GameWidget::onGameTick() {
    if (paused_ || game_.game_over()) {
        return;
    }
    game_.tick();
    game_timer_->setInterval(tickIntervalMs());
    emit scoreChanged(game_.score());
    emit gameEnded(game_.game_over());
    update();
}

void GameWidget::onAnimTick() {
    pulse_phase_ += 0.08;
    if (pulse_phase_ > 6.28318530718) {
        pulse_phase_ -= 6.28318530718;
    }
    update();
}

void GameWidget::paintEvent(QPaintEvent* /*event*/) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setRenderHint(QPainter::SmoothPixmapTransform, true);

    QLinearGradient bg(0, 0, width(), height());
    bg.setColorAt(0.0, QColor(8, 12, 22));
    bg.setColorAt(0.45, QColor(14, 22, 38));
    bg.setColorAt(1.0, QColor(6, 10, 18));
    p.fillRect(rect(), bg);

    QRadialGradient vignette(width() / 2.0, height() / 2.0, std::max(width(), height()) * 0.65);
    vignette.setColorAt(0.0, QColor(0, 0, 0, 0));
    vignette.setColorAt(1.0, QColor(0, 0, 0, 120));
    p.fillRect(rect(), vignette);

    const QRectF fr = fieldRect();
    QPainterPath field_path;
    field_path.addRoundedRect(fr, 10.0, 10.0);
    p.fillPath(field_path, QColor(12, 20, 32, 220));
    p.setPen(QPen(QColor(0, 255, 200, 55), 1.5));
    p.drawPath(field_path);

    const qreal pad = 2.5;
    const qreal radius = 4.0;

    for (int gy = 0; gy < k_grid_h; ++gy) {
        for (int gx = 0; gx < k_grid_w; ++gx) {
            const Tile t = game_.tile_at(gx, gy);
            if (t == Tile::Empty) {
                continue;
            }
            QRectF r = cellRect(gx, gy);
            r.adjust(pad, pad, -pad, -pad);
            if (r.width() <= 0 || r.height() <= 0) {
                continue;
            }

            if (t == Tile::Food) {
                const double pulse = 0.55 + 0.45 * std::sin(pulse_phase_);
                const QPointF c = r.center();
                const double pr = std::min(r.width(), r.height()) * 0.42 * pulse;
                QRadialGradient g(c, pr * 1.4);
                g.setColorAt(0.0, QColor(255, 120, 160));
                g.setColorAt(0.55, QColor(255, 60, 100));
                g.setColorAt(1.0, QColor(120, 20, 50, 0));
                p.setPen(Qt::NoPen);
                p.setBrush(g);
                p.drawEllipse(c, static_cast<qreal>(pr), static_cast<qreal>(pr));
                p.setPen(QPen(QColor(255, 200, 220, 180), 1.2));
                p.setBrush(Qt::NoBrush);
                p.drawEllipse(c, static_cast<qreal>(pr * 0.85), static_cast<qreal>(pr * 0.85));
                continue;
            }

            const int dist = game_.distance_from_head(gx, gy);
            const int max_i = static_cast<int>(std::max<std::size_t>(1, game_.snake_length()) - 1);
            const double tgrad = max_i > 0 ? static_cast<double>(dist) / static_cast<double>(max_i) : 0.0;

            const QColor head_hi(220, 255, 250);
            const QColor head_lo(0, 255, 210);
            const QColor tail(0, 90, 70);
            QColor body = QColor::fromRgbF(
                head_lo.redF() * (1.0 - tgrad) + tail.redF() * tgrad,
                head_lo.greenF() * (1.0 - tgrad) + tail.greenF() * tgrad,
                head_lo.blueF() * (1.0 - tgrad) + tail.blueF() * tgrad);

            if (t == Tile::SnakeHead) {
                body = head_lo;
            }

            QLinearGradient seg(r.topLeft(), r.bottomRight());
            seg.setColorAt(0.0, t == Tile::SnakeHead ? head_hi : body.lighter(115));
            seg.setColorAt(1.0, body.darker(108));

            p.setPen(QPen(body.darker(140), 1.0));
            p.setBrush(seg);
            p.drawRoundedRect(r, radius, radius);

            if (t == Tile::SnakeHead) {
                const QPointF eye_off(r.width() * 0.22, r.height() * 0.18);
                p.setBrush(QColor(10, 30, 25));
                p.setPen(Qt::NoPen);
                p.drawEllipse(r.center() - eye_off, r.width() * 0.1, r.height() * 0.1);
                p.drawEllipse(r.center() + QPointF(eye_off.x(), -eye_off.y()), r.width() * 0.1, r.height() * 0.1);
            }
        }
    }

    // Subtle scanlines (retro LCD)
    p.setCompositionMode(QPainter::CompositionMode_Multiply);
    p.setPen(QPen(QColor(0, 0, 0, 28), 1));
    for (int y = static_cast<int>(fr.top()); y < fr.bottom(); y += 3) {
        p.drawLine(QPointF(fr.left(), y), QPointF(fr.right(), y));
    }
    p.setCompositionMode(QPainter::CompositionMode_SourceOver);

    if (paused_ && !game_.game_over()) {
        p.fillRect(rect(), QColor(0, 0, 0, 110));
        p.setPen(QColor(0, 255, 210));
        p.setFont(QFont(QStringLiteral("Helvetica Neue"), 22, QFont::Bold));
        p.drawText(rect(), Qt::AlignCenter, QStringLiteral("PAUSED"));
    }

    if (game_.game_over()) {
        p.fillRect(rect(), QColor(8, 10, 18, 200));
        p.setPen(QColor(255, 90, 120));
        p.setFont(QFont(QStringLiteral("Helvetica Neue"), 26, QFont::Bold));
        p.drawText(rect(), Qt::AlignCenter, QStringLiteral("GAME OVER\nPress R to play again"));
    }
}

void GameWidget::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_P) {
        paused_ = !paused_;
        emit pausedChanged(paused_);
        update();
        return;
    }
    if (game_.game_over() && (event->key() == Qt::Key_R)) {
        restartGame();
        return;
    }
    if (paused_ || game_.game_over()) {
        QWidget::keyPressEvent(event);
        return;
    }

    switch (event->key()) {
        case Qt::Key_Up:
        case Qt::Key_W:
            game_.set_next_direction(Direction::Up);
            break;
        case Qt::Key_Down:
        case Qt::Key_S:
            game_.set_next_direction(Direction::Down);
            break;
        case Qt::Key_Left:
        case Qt::Key_A:
            game_.set_next_direction(Direction::Left);
            break;
        case Qt::Key_Right:
        case Qt::Key_D:
            game_.set_next_direction(Direction::Right);
            break;
        default:
            QWidget::keyPressEvent(event);
            return;
    }
}
