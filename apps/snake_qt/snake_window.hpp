#ifndef MB_CPP_APP_TEMPLATE_SNAKE_WINDOW_HPP_INCLUDED
#define MB_CPP_APP_TEMPLATE_SNAKE_WINDOW_HPP_INCLUDED

#include <QMainWindow>

class GameWidget;
class QLabel;

class SnakeWindow final : public QMainWindow {
    Q_OBJECT

public:
    explicit SnakeWindow(QWidget* parent = nullptr);

private slots:
    void onScoreChanged(int score);
    void onGameEnded(bool over);
    void onPausedChanged(bool paused);

private:
    void applyStyle();

    bool game_over_{false};

    QLabel* score_label_{};
    QLabel* status_label_{};
    GameWidget* game_{};
};

#endif
