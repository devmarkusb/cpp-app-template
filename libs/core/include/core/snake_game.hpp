#ifndef MB_CPP_APP_TEMPLATE_SNAKE_GAME_HPP_INCLUDED
#define MB_CPP_APP_TEMPLATE_SNAKE_GAME_HPP_INCLUDED

#include <cstdint>
#include <deque>
#include <random>

namespace mb::cpp_app_template {
enum class Direction : std::uint8_t {
    Up,
    Down,
    Left,
    Right
};

enum class Tile : std::uint8_t {
    Empty,
    SnakeHead,
    SnakeBody,
    Food
};

struct Point {
    int x{};
    int y{};

    auto operator==(const Point& o) const noexcept -> bool {
        return x == o.x && y == o.y;
    }
};

/// Grid-based Snake game state (Nokia-style: walls, no wrap; opposite turns ignored).
class SnakeGame {
public:
    SnakeGame(int width, int height, std::uint32_t random_seed);

    [[nodiscard]] auto width() const noexcept -> int {
        return width_;
    }

    [[nodiscard]] auto height() const noexcept -> int {
        return height_;
    }

    [[nodiscard]] auto score() const noexcept -> int {
        return score_;
    }

    [[nodiscard]] auto game_over() const noexcept -> bool {
        return game_over_;
    }

    void reset();
    void set_next_direction(Direction d);

    /// Advance one step. Returns false if the game is over after this tick.
    auto tick() -> bool;

    [[nodiscard]] auto tile_at(int x, int y) const -> Tile;

    /// Number of segments (head + body).
    [[nodiscard]] auto snake_length() const noexcept -> std::size_t;

    /// Index along the snake from head (0) to tail; \c -1 if \p (x,y) is not on the snake.
    [[nodiscard]] auto distance_from_head(int x, int y) const -> int;

private:
    [[nodiscard]] auto head() const -> Point;
    static auto delta(Direction d) noexcept -> Point;
    auto place_food() -> bool;

    int width_{};
    int height_{};
    int score_{};
    bool game_over_{};
    Direction direction_{Direction::Right};
    Direction next_direction_{Direction::Right};
    std::deque<Point> snake_;
    Point food_{};
    bool has_food_{};
    std::mt19937 rng_;
};
} // namespace mb::cpp_app_template

#endif
