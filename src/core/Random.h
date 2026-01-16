#pragma once

#include <cstdint>
#include "../utils/MathUtils.h"

namespace BattleCity {

// Deterministic random number generator for game consistency
class Random {
private:
    uint32_t seed_;

public:
    Random(uint32_t seed = 0x12345678) : seed_(seed) {}

    // Generate next random number
    uint32_t next() {
        // Linear Congruential Generator (same as NES)
        seed_ = seed_ * 1103515245 + 12345;
        return seed_ >> 16; // Return upper 16 bits
    }

    // Generate random integer in range [min, max]
    int range(int min, int max) {
        return min + (next() % (max - min + 1));
    }

    // Generate random float in range [0, 1)
    float nextFloat() {
        return static_cast<float>(next()) / 65536.0f;
    }

    // Set new seed
    void setSeed(uint32_t seed) {
        seed_ = seed;
    }

    // Get current seed
    uint32_t getSeed() const {
        return seed_;
    }

    // Generate random direction for AI
    Direction randomDirection() {
        int dir = range(0, 3);
        switch (dir) {
            case 0: return Direction::UP;
            case 1: return Direction::DOWN;
            case 2: return Direction::LEFT;
            case 3: return Direction::RIGHT;
            default: return Direction::UP;
        }
    }

    // Generate random boolean with given probability (0-100)
    bool randomBool(int probabilityPercent) {
        return range(0, 99) < probabilityPercent;
    }
};

} // namespace BattleCity