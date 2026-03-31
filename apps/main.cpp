#include "core/snake_game.hpp"

#if defined(__APPLE__)

#include <curses.h>

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <random>

namespace {
using mb::cpp_app_template::Direction;
using mb::cpp_app_template::SnakeGame;
using mb::cpp_app_template::Tile;

auto now_ms() -> std::chrono::steady_clock::time_point {
    return std::chrono::steady_clock::now();
}

void draw_cell(int row, int col, Tile t) {
    int pair = 1;
    char c = ' ';
    bool bold = false;
    switch (t) {
        case Tile::Empty:
            c = '.';
            pair = 1;
            break;
        case Tile::SnakeHead:
            c = 'O';
            pair = 2;
            bold = true;
            break;
        case Tile::SnakeBody:
            c = 'o';
            pair = 3;
            break;
        case Tile::Food:
            c = '*';
            pair = 4;
            bold = true;
            break;
    }
    if (bold) {
        attron(COLOR_PAIR(pair) | A_BOLD);
    } else {
        attron(COLOR_PAIR(pair));
    }
    mvaddch(row, col, static_cast<unsigned char>(c));
    if (bold) {
        attroff(COLOR_PAIR(pair) | A_BOLD);
    } else {
        attroff(COLOR_PAIR(pair));
    }
}

void draw_game(const SnakeGame& g, int base_y, int base_x) {
    for (int gy = 0; gy < g.height(); ++gy) {
        for (int gx = 0; gx < g.width(); ++gx) {
            draw_cell(base_y + gy, base_x + gx, g.tile_at(gx, gy));
        }
    }
}

void draw_frame(int base_y, int base_x, int w, int h) {
    attron(COLOR_PAIR(5));
    for (int x = 0; x < w + 2; ++x) {
        mvaddch(base_y - 1, base_x - 1 + x, '#');
        mvaddch(base_y + h, base_x - 1 + x, '#');
    }
    for (int y = 0; y < h; ++y) {
        mvaddch(base_y + y, base_x - 1, '#');
        mvaddch(base_y + y, base_x + w, '#');
    }
    attroff(COLOR_PAIR(5));
}

auto tick_delay_ms(int score) -> int {
    const int base = 140;
    const int bonus = std::min(90, score / 2);
    return std::max(35, base - bonus);
}
} // namespace

int main() {
    constexpr int min_cols = 46;
    constexpr int min_lines = 22;

    initscr();
    if (COLS < min_cols || LINES < min_lines) {
        endwin();
        std::fprintf(
            stderr, "Terminal too small (have %dx%d, need at least %dx%d).\n", COLS, LINES, min_cols, min_lines);
        return EXIT_FAILURE;
    }

    cbreak();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    start_color();
    use_default_colors();
    init_pair(1, COLOR_GREEN, -1); // LCD background tint (empty)
    init_pair(2, COLOR_WHITE, -1); // head
    init_pair(3, COLOR_GREEN, -1); // body (dimmer via non-bold)
    init_pair(4, COLOR_YELLOW, -1); // food
    init_pair(5, COLOR_GREEN, COLOR_BLACK); // border

    std::random_device rd;
    SnakeGame game(32, 16, rd());

    const int base_x = (COLS - game.width() - 2) / 2 + 1;
    const int base_y = (LINES - game.height() - 4) / 2 + 1;

    bool paused = false;
    bool quit = false;
    auto last_tick = now_ms();
    int delay = tick_delay_ms(game.score());

    while (!quit) {
        const int ch = getch();
        if (ch == 'q' || ch == 'Q') {
            quit = true;
            break;
        }
        if (ch == 'p' || ch == 'P') {
            paused = !paused;
        }
        if (game.game_over() && (ch == 'r' || ch == 'R')) {
            game.reset();
            last_tick = now_ms();
            delay = tick_delay_ms(game.score());
        }

        if (!paused && !game.game_over()) {
            switch (ch) {
                case KEY_UP:
                case 'w':
                case 'W':
                    game.set_next_direction(Direction::Up);
                    break;
                case KEY_DOWN:
                case 's':
                case 'S':
                    game.set_next_direction(Direction::Down);
                    break;
                case KEY_LEFT:
                case 'a':
                case 'A':
                    game.set_next_direction(Direction::Left);
                    break;
                case KEY_RIGHT:
                case 'd':
                case 'D':
                    game.set_next_direction(Direction::Right);
                    break;
                default:
                    break;
            }

            const auto t = now_ms();
            if (std::chrono::duration_cast<std::chrono::milliseconds>(t - last_tick).count() >= delay) {
                game.tick();
                last_tick = t;
                delay = tick_delay_ms(game.score());
            }
        }

        erase();
        attron(COLOR_PAIR(2) | A_BOLD);
        mvprintw(base_y - 3, (COLS - 7) / 2, " SNAKE ");
        attroff(COLOR_PAIR(2) | A_BOLD);
        attron(COLOR_PAIR(3));
        mvprintw(base_y - 2, (COLS - 40) / 2, " Score %6d   WASD / arrows   p pause   q quit ", game.score());
        attroff(COLOR_PAIR(3));

        draw_frame(base_y, base_x, game.width(), game.height());
        draw_game(game, base_y, base_x);

        if (paused) {
            attron(COLOR_PAIR(2) | A_BOLD);
            mvprintw(base_y + game.height() / 2, (COLS - 10) / 2, " P A U S E ");
            attroff(A_BOLD);
        }
        if (game.game_over()) {
            attron(COLOR_PAIR(4) | A_BOLD);
            mvprintw(base_y + game.height() / 2 + 1, (COLS - 28) / 2, " GAME OVER  —  r retry ");
            attroff(A_BOLD);
        }

        refresh();
        napms(12);
    }

    endwin();
    return EXIT_SUCCESS;
}

#else

#include <cstdio>

int main() {
    std::fprintf(stderr, "This Snake build is supported on macOS only (terminal + curses).\n");
    return 1;
}

#endif
