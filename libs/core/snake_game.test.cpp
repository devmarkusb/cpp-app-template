#include "core/snake_game.hpp"

#include "gtest/gtest.h"

namespace mb::cpp_app_template {
namespace {
TEST(SnakeGameTest, reset_spawns_snake_and_food) {
    SnakeGame g(12, 8, 12'345);
    EXPECT_EQ(g.width(), 12);
    EXPECT_EQ(g.height(), 8);
    EXPECT_FALSE(g.game_over());
    EXPECT_EQ(g.score(), 0);
    int heads = 0;
    int foods = 0;
    for (int y = 0; y < g.height(); ++y) {
        for (int x = 0; x < g.width(); ++x) {
            const Tile t = g.tile_at(x, y);
            if (t == Tile::SnakeHead) {
                ++heads;
            }
            if (t == Tile::Food) {
                ++foods;
            }
        }
    }
    EXPECT_EQ(heads, 1);
    EXPECT_EQ(foods, 1);
}

TEST(SnakeGameTest, tick_moves_head) {
    SnakeGame g(12, 8, 1);
    const int hx0 = g.width() / 2;
    const int hy0 = g.height() / 2;
    EXPECT_EQ(g.tile_at(hx0, hy0), Tile::SnakeHead);
    g.set_next_direction(Direction::Right);
    ASSERT_TRUE(g.tick());
    EXPECT_EQ(g.tile_at(hx0 + 1, hy0), Tile::SnakeHead);
}

TEST(SnakeGameTest, wall_ends_game) {
    SnakeGame g(6, 4, 99);
    g.set_next_direction(Direction::Up);
    for (int i = 0; i < 10; ++i) {
        if (!g.tick()) {
            break;
        }
    }
    EXPECT_TRUE(g.game_over());
}

TEST(SnakeGameTest, eating_increases_score) {
    for (std::uint32_t seed = 0; seed < 30'000; ++seed) {
        SnakeGame g(24, 18, seed);
        int hx = -1;
        int hy = -1;
        int fx = -1;
        int fy = -1;
        for (int y = 0; y < g.height(); ++y) {
            for (int x = 0; x < g.width(); ++x) {
                if (g.tile_at(x, y) == Tile::SnakeHead) {
                    hx = x;
                    hy = y;
                }
                if (g.tile_at(x, y) == Tile::Food) {
                    fx = x;
                    fy = y;
                }
            }
        }
        const int dx = fx - hx;
        const int dy = fy - hy;
        const bool ortho_adjacent = (dx == 0 && (dy == 1 || dy == -1)) || (dy == 0 && (dx == 1 || dx == -1));
        if (!ortho_adjacent) {
            continue;
        }
        Direction d = Direction::Right;
        if (dx == 1) {
            d = Direction::Right;
        } else if (dx == -1) {
            d = Direction::Left;
        } else if (dy == 1) {
            d = Direction::Down;
        } else if (dy == -1) {
            d = Direction::Up;
        }
        g.set_next_direction(d);
        ASSERT_TRUE(g.tick());
        EXPECT_GT(g.score(), 0);
        return;
    }
    FAIL() << "no seed with food orthogonally adjacent to head";
}

TEST(SnakeGameTest, opposite_key_ignored) {
    SnakeGame g(12, 8, 7);
    g.set_next_direction(Direction::Left); // opposite of initial Right
    g.tick();
    // Still moving right (next was ignored)
    const int hx = g.width() / 2 + 1;
    const int hy = g.height() / 2;
    EXPECT_EQ(g.tile_at(hx, hy), Tile::SnakeHead);
}
} // namespace
} // namespace mb::cpp_app_template
