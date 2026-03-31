#include "core/snake_game.hpp"

#include <algorithm>

namespace mb::cpp_app_template {
namespace {
auto opposite(Direction a, Direction b) noexcept -> bool {
    return (a == Direction::Up && b == Direction::Down) || (a == Direction::Down && b == Direction::Up)
           || (a == Direction::Left && b == Direction::Right) || (a == Direction::Right && b == Direction::Left);
}
} // namespace

SnakeGame::SnakeGame(int width, int height, std::uint32_t random_seed)
    : width_{width}
    , height_{height}
    , rng_{random_seed} {
    reset();
}

void SnakeGame::reset() {
    score_ = 0;
    game_over_ = false;
    direction_ = Direction::Right;
    next_direction_ = Direction::Right;
    snake_.clear();

    const int mid_y = height_ / 2;
    const int mid_x = width_ / 2;
    snake_.push_back(Point{mid_x, mid_y});
    snake_.push_back(Point{mid_x - 1, mid_y});
    snake_.push_back(Point{mid_x - 2, mid_y});

    has_food_ = place_food();
    if (!has_food_) {
        game_over_ = true;
    }
}

auto SnakeGame::delta(Direction d) noexcept -> Point {
    switch (d) {
        case Direction::Up:
            return {0, -1};
        case Direction::Down:
            return {0, 1};
        case Direction::Left:
            return {-1, 0};
        case Direction::Right:
            return {1, 0};
    }
    return {};
}

void SnakeGame::set_next_direction(Direction d) {
    if (!opposite(direction_, d)) {
        next_direction_ = d;
    }
}

auto SnakeGame::head() const -> Point {
    return snake_.front();
}

auto SnakeGame::place_food() -> bool {
    std::vector<Point> empty;
    empty.reserve(static_cast<std::size_t>(width_ * height_));
    for (int y = 0; y < height_; ++y) {
        for (int x = 0; x < width_; ++x) {
            const Point p{x, y};
            const bool on_snake = std::any_of(snake_.begin(), snake_.end(), [&](const Point& s) {
                return s == p;
            });
            if (!on_snake) {
                empty.push_back(p);
            }
        }
    }
    if (empty.empty()) {
        return false;
    }
    std::uniform_int_distribution<std::size_t> dist(0, empty.size() - 1);
    food_ = empty[dist(rng_)];
    return true;
}

auto SnakeGame::tick() -> bool {
    if (game_over_) {
        return false;
    }

    direction_ = next_direction_;
    const Point new_head{head().x + delta(direction_).x, head().y + delta(direction_).y};

    if (new_head.x < 0 || new_head.x >= width_ || new_head.y < 0 || new_head.y >= height_) {
        game_over_ = true;
        return false;
    }

    const bool eating = has_food_ && new_head == food_;

    if (!eating) {
        for (std::size_t i = 0; i + 1 < snake_.size(); ++i) {
            if (snake_[i] == new_head) {
                game_over_ = true;
                return false;
            }
        }
    }

    snake_.push_front(new_head);
    if (eating) {
        score_ += 10;
        has_food_ = place_food();
        if (!has_food_) {
            game_over_ = true;
            return false;
        }
    } else {
        snake_.pop_back();
    }
    return !game_over_;
}

auto SnakeGame::tile_at(int x, int y) const -> Tile {
    if (x < 0 || x >= width_ || y < 0 || y >= height_) {
        return Tile::Empty;
    }
    const Point p{x, y};
    if (has_food_ && p == food_) {
        return Tile::Food;
    }
    if (snake_.empty()) {
        return Tile::Empty;
    }
    if (p == snake_.front()) {
        return Tile::SnakeHead;
    }
    if (std::any_of(snake_.begin() + 1, snake_.end(), [&](const Point& s) {
            return s == p;
        })) {
        return Tile::SnakeBody;
    }
    return Tile::Empty;
}

auto SnakeGame::snake_length() const noexcept -> std::size_t {
    return snake_.size();
}

auto SnakeGame::distance_from_head(int x, int y) const -> int {
    const Point p{x, y};
    for (std::size_t i = 0; i < snake_.size(); ++i) {
        if (snake_[i] == p) {
            return static_cast<int>(i);
        }
    }
    return -1;
}
} // namespace mb::cpp_app_template
