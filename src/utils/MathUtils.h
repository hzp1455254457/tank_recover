#pragma once

#include <cstdint>
#include <cmath>
#include <algorithm>

namespace BattleCity {

// Vector2 with sub-pixel precision
struct Vector2 {
    int32_t x;  // Position * 256 (sub-pixel precision)
    int32_t y;

    Vector2() : x(0), y(0) {}
    Vector2(int32_t x, int32_t y) : x(x), y(y) {}
    Vector2(float fx, float fy) : x(static_cast<int32_t>(fx * 256)), y(static_cast<int32_t>(fy * 256)) {}

    // Convert to pixel coordinates
    int pixelX() const { return x >> 8; }
    int pixelY() const { return y >> 8; }

    // Convert from pixel coordinates
    static Vector2 fromPixels(int px, int py) {
        return Vector2(px << 8, py << 8);
    }

    // Arithmetic operations
    Vector2 operator+(const Vector2& other) const {
        return Vector2(x + other.x, y + other.y);
    }

    Vector2 operator-(const Vector2& other) const {
        return Vector2(x - other.x, y - other.y);
    }

    Vector2& operator+=(const Vector2& other) {
        x += other.x;
        y += other.y;
        return *this;
    }

    Vector2& operator-=(const Vector2& other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    // Distance calculation
    float distance(const Vector2& other) const {
        int32_t dx = x - other.x;
        int32_t dy = y - other.y;
        return std::sqrt(dx * dx + dy * dy) / 256.0f;
    }

    // Manhattan distance (for AI pathfinding)
    int manhattanDistance(const Vector2& other) const {
        return std::abs(pixelX() - other.pixelX()) + std::abs(pixelY() - other.pixelY());
    }
};

// Rectangle structure for collision detection
struct Rect {
    int x, y, w, h;

    Rect() : x(0), y(0), w(0), h(0) {}
    Rect(int x, int y, int w, int h) : x(x), y(y), w(w), h(h) {}

    bool intersects(const Rect& other) const {
        return !(x + w <= other.x || other.x + other.w <= x ||
                 y + h <= other.y || other.y + other.h <= y);
    }

    bool contains(int px, int py) const {
        return px >= x && px < x + w && py >= y && py < y + h;
    }
};

// Direction enumeration
enum class Direction {
    UP,
    DOWN,
    LEFT,
    RIGHT,
    NONE
};

// Game action enumeration
enum class GameAction {
    UP,
    DOWN,
    LEFT,
    RIGHT,
    SHOOT,
    START,
    PAUSE,
    QUIT
};

// Game state enumeration
enum class GameState {
    MENU,
    PLAYING,
    PAUSED,
    GAME_OVER,
    LEVEL_COMPLETE,
    DEMO
};

// Enemy tank types
enum class EnemyType {
    BASIC,
    FAST,
    HEAVY,
    ELITE
};

// Terrain types
enum class TerrainType {
    GRASS,
    BRICK,
    STEEL,
    WATER,
    BASE_BRICK
};


// Utility functions
class MathUtils {
public:
    static constexpr float PI = 3.14159265358979323846f;

    // Clamp value between min and max
    template<typename T>
    static T clamp(T value, T min, T max) {
        if (value < min) return min;
        if (value > max) return max;
        return value;
    }

    // Linear interpolation
    static float lerp(float a, float b, float t) {
        return a + (b - a) * t;
    }

    // Check if point is within circle
    static bool pointInCircle(int px, int py, int cx, int cy, int radius) {
        int dx = px - cx;
        int dy = py - cy;
        return dx * dx + dy * dy <= radius * radius;
    }

    // Convert direction to velocity
    static Vector2 directionToVelocity(Direction dir, int speed) {
        switch (dir) {
            case Direction::UP:    return Vector2(0, -speed);
            case Direction::DOWN:  return Vector2(0, speed);
            case Direction::LEFT:  return Vector2(-speed, 0);
            case Direction::RIGHT: return Vector2(speed, 0);
            default:               return Vector2(0, 0);
        }
    }

    // Get opposite direction
    static Direction getOppositeDirection(Direction dir) {
        switch (dir) {
            case Direction::UP:    return Direction::DOWN;
            case Direction::DOWN:  return Direction::UP;
            case Direction::LEFT:  return Direction::RIGHT;
            case Direction::RIGHT: return Direction::LEFT;
            default:               return Direction::NONE;
        }
    }

    // Calculate distance between two points
    static int distance(int x1, int y1, int x2, int y2) {
        int dx = x2 - x1;
        int dy = y2 - y1;
        return static_cast<int>(std::sqrt(dx * dx + dy * dy));
    }

    // Check if two rectangles intersect
    static bool rectsIntersect(const Rect& r1, const Rect& r2) {
        return !(r1.x + r1.w <= r2.x ||
                 r2.x + r2.w <= r1.x ||
                 r1.y + r1.h <= r2.y ||
                 r2.y + r2.h <= r1.y);
    }
};

} // namespace BattleCity