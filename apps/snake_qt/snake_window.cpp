#include "snake_window.hpp"

#include "game_widget.hpp"

#include <QApplication>
#include <QFrame>
#include <QLabel>
#include <QMenuBar>
#include <QVBoxLayout>
#include <QWidget>

SnakeWindow::SnakeWindow(QWidget* parent)
    : QMainWindow(parent) {
    setWindowTitle(QStringLiteral("Snake"));
    resize(880, 640);

    auto* central = new QWidget(this);
    setCentralWidget(central);

    auto* layout = new QVBoxLayout(central);
    layout->setContentsMargins(20, 16, 20, 16);
    layout->setSpacing(12);

    auto* title = new QLabel(QStringLiteral("SNAKE"), central);
    title->setObjectName(QStringLiteral("titleLabel"));
    title->setAlignment(Qt::AlignCenter);

    score_label_ = new QLabel(QStringLiteral("SCORE  0"), central);
    score_label_->setObjectName(QStringLiteral("scoreLabel"));
    score_label_->setAlignment(Qt::AlignCenter);

    status_label_ = new QLabel(QStringLiteral("WASD / arrows  ·  P pause  ·  R restart when over"), central);
    status_label_->setObjectName(QStringLiteral("hintLabel"));
    status_label_->setAlignment(Qt::AlignCenter);

    auto* line = new QFrame(central);
    line->setFrameShape(QFrame::HLine);
    line->setObjectName(QStringLiteral("sep"));

    game_ = new GameWidget(central);
    layout->addWidget(title);
    layout->addWidget(score_label_);
    layout->addWidget(status_label_);
    layout->addWidget(line);
    layout->addWidget(game_, 1);

    connect(game_, &GameWidget::scoreChanged, this, &SnakeWindow::onScoreChanged);
    connect(game_, &GameWidget::gameEnded, this, &SnakeWindow::onGameEnded);
    connect(game_, &GameWidget::pausedChanged, this, &SnakeWindow::onPausedChanged);

    onScoreChanged(game_->currentScore());
    game_over_ = game_->isGameOver();
    onGameEnded(game_over_);

    auto* game_menu = menuBar()->addMenu(QStringLiteral("Game"));
    auto* act_new = game_menu->addAction(QStringLiteral("New game"));
    act_new->setShortcut(QStringLiteral("Ctrl+N"));
    connect(act_new, &QAction::triggered, game_, &GameWidget::restartGame);

    auto* act_pause = game_menu->addAction(QStringLiteral("Pause"));
    act_pause->setShortcut(QStringLiteral("Ctrl+P"));
    connect(act_pause, &QAction::triggered, game_, &GameWidget::togglePause);

    game_menu->addSeparator();
    auto* act_quit = game_menu->addAction(QStringLiteral("Quit"));
    act_quit->setShortcut(QStringLiteral("Ctrl+Q"));
    connect(act_quit, &QAction::triggered, qApp, &QApplication::quit);

    applyStyle();
}

void SnakeWindow::applyStyle() {
    setStyleSheet(QStringLiteral(
        "QMainWindow { background-color: #070b12; }"
        "QLabel#titleLabel {"
        "  color: #58f5c2;"
        "  font-size: 32px;"
        "  font-weight: 800;"
        "  letter-spacing: 0.35em;"
        "}"
        "QLabel#scoreLabel {"
        "  color: #a8fff0;"
        "  font-size: 18px;"
        "  font-weight: 600;"
        "  font-variant-numeric: tabular-nums;"
        "}"
        "QLabel#hintLabel {"
        "  color: #5a7a8c;"
        "  font-size: 13px;"
        "}"
        "QFrame#sep {"
        "  color: #1e3a44;"
        "  max-height: 1px;"
        "}"
        "QMenuBar {"
        "  background-color: #0a1018;"
        "  color: #8ab4c4;"
        "}"
        "QMenuBar::item:selected { background-color: #152028; }"
        "QMenu {"
        "  background-color: #121a24;"
        "  color: #c8e0e8;"
        "  border: 1px solid #1e3a44;"
        "}"
        "QMenu::item:selected { background-color: #1a3040; }"));
}

void SnakeWindow::onScoreChanged(int score) {
    score_label_->setText(QStringLiteral("SCORE  %1").arg(score, 6, 10, QChar(QLatin1Char(' '))));
}

void SnakeWindow::onGameEnded(bool over) {
    game_over_ = over;
    status_label_->setText(
        over ? QStringLiteral("You crashed — press R or Game → New game")
             : QStringLiteral("WASD / arrows  ·  P pause  ·  R restart when over"));
}

void SnakeWindow::onPausedChanged(bool paused) {
    if (paused) {
        status_label_->setText(QStringLiteral("Paused — P to continue"));
    } else if (!game_over_) {
        status_label_->setText(QStringLiteral("WASD / arrows  ·  P pause  ·  R restart when over"));
    }
}
