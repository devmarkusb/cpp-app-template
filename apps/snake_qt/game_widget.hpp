#ifndef MB_CPP_APP_TEMPLATE_GAME_WIDGET_HPP_INCLUDED
#define MB_CPP_APP_TEMPLATE_GAME_WIDGET_HPP_INCLUDED

#include "core/snake_game.hpp"

#include <QWidget>

#include <memory>

class QTimer;

class GameWidget final : public QWidget {
    Q_OBJECT

public:
    explicit GameWidget(QWidget* parent = nullptr);
    void restartGame();
    void togglePause();

    [[nodiscard]] auto currentScore() const -> int {
        return game_.score();
    }

    [[nodiscard]] auto isGameOver() const -> bool {
        return game_.game_over();
    }

signals:
    void scoreChanged(int score);
    void gameEnded(bool over);
    void pausedChanged(bool paused);

protected:
    void paintEvent(QPaintEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private slots:
    void onGameTick();
    void onAnimTick();

private:
    [[nodiscard]] QRectF fieldRect() const;
    [[nodiscard]] QRectF cellRect(int gx, int gy) const;
    [[nodiscard]] int tickIntervalMs() const;

    mb::cpp_app_template::SnakeGame game_{32, 16, 0};
    QTimer* game_timer_{};
    QTimer* anim_timer_{};
    double pulse_phase_{0.0};
    bool paused_{false};
};

#endif
