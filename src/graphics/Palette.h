#pragma once

#include <array>
#include <SDL.h>

namespace BattleCity {

// NES Color Palette (54 colors exactly matching the original)
class Palette {
private:
    std::array<SDL_Color, 64> nesColors_;

public:
    Palette() {
        initNesPalette();
    }

    // Get NES color by index (0-63)
    const SDL_Color& getColor(uint8_t index) const {
        return nesColors_[index & 0x3F]; // Mask to valid range
    }

    // Get raw color data
    const SDL_Color* getColors() const {
        return nesColors_.data();
    }

    // Get palette size
    size_t size() const {
        return nesColors_.size();
    }

private:
    void initNesPalette() {
        // NES System Palette - Exact RGB values from NES PPU
        nesColors_[0x00] = {84, 84, 84, 255};     // Dark gray
        nesColors_[0x01] = {0, 30, 116, 255};     // Dark blue
        nesColors_[0x02] = {8, 16, 144, 255};     // Blue
        nesColors_[0x03] = {48, 0, 136, 255};     // Purple-blue
        nesColors_[0x04] = {68, 0, 100, 255};     // Purple
        nesColors_[0x05] = {92, 0, 48, 255};      // Dark magenta
        nesColors_[0x06] = {84, 4, 0, 255};       // Dark red
        nesColors_[0x07] = {60, 24, 0, 255};      // Brown
        nesColors_[0x08] = {32, 42, 0, 255};      // Dark green
        nesColors_[0x09] = {8, 58, 0, 255};       // Green
        nesColors_[0x0A] = {0, 64, 0, 255};       // Light green
        nesColors_[0x0B] = {0, 60, 0, 255};       // Bright green
        nesColors_[0x0C] = {0, 50, 60, 255};      // Dark cyan
        nesColors_[0x0D] = {0, 0, 0, 255};        // Black
        nesColors_[0x0E] = {0, 0, 0, 255};        // Black (duplicate)
        nesColors_[0x0F] = {0, 0, 0, 255};        // Black (duplicate)

        nesColors_[0x10] = {152, 150, 152, 255};  // Light gray
        nesColors_[0x11] = {8, 76, 196, 255};     // Light blue
        nesColors_[0x12] = {48, 50, 236, 255};    // Bright blue
        nesColors_[0x13] = {92, 30, 228, 255};    // Light purple
        nesColors_[0x14] = {136, 20, 176, 255};   // Magenta
        nesColors_[0x15] = {160, 20, 100, 255};   // Pink
        nesColors_[0x16] = {152, 34, 32, 255};    // Light red
        nesColors_[0x17] = {120, 60, 0, 255};     // Orange
        nesColors_[0x18] = {84, 90, 0, 255};      // Yellow-green
        nesColors_[0x19] = {40, 114, 0, 255};     // Light green
        nesColors_[0x1A] = {8, 124, 0, 255};      // Bright green
        nesColors_[0x1B] = {0, 118, 40, 255};     // Cyan-green
        nesColors_[0x1C] = {0, 102, 120, 255};    // Light cyan
        nesColors_[0x1D] = {0, 0, 0, 255};        // Black
        nesColors_[0x1E] = {0, 0, 0, 255};        // Black
        nesColors_[0x1F] = {0, 0, 0, 255};        // Black

        nesColors_[0x20] = {236, 238, 236, 255};  // White
        nesColors_[0x21] = {76, 154, 236, 255};   // Light blue
        nesColors_[0x22] = {120, 124, 236, 255};  // Pale blue
        nesColors_[0x23] = {176, 98, 236, 255};   // Light purple
        nesColors_[0x24] = {228, 84, 236, 255};   // Light magenta
        nesColors_[0x25] = {236, 88, 180, 255};   // Light pink
        nesColors_[0x26] = {236, 106, 100, 255};  // Peach
        nesColors_[0x27] = {212, 136, 32, 255};   // Light orange
        nesColors_[0x28] = {160, 170, 0, 255};    // Yellow
        nesColors_[0x29] = {116, 196, 0, 255};    // Light yellow-green
        nesColors_[0x2A] = {76, 208, 32, 255};    // Light green
        nesColors_[0x2B] = {56, 204, 108, 255};   // Light cyan-green
        nesColors_[0x2C] = {56, 180, 204, 255};   // Light cyan
        nesColors_[0x2D] = {60, 60, 60, 255};     // Dark gray
        nesColors_[0x2E] = {0, 0, 0, 255};        // Black
        nesColors_[0x2F] = {0, 0, 0, 255};        // Black

        nesColors_[0x30] = {236, 238, 236, 255};  // White
        nesColors_[0x31] = {168, 204, 236, 255};  // Very light blue
        nesColors_[0x32] = {188, 188, 236, 255};  // Pale lavender
        nesColors_[0x33] = {212, 178, 236, 255};  // Light lavender
        nesColors_[0x34] = {236, 174, 236, 255};  // Light pink
        nesColors_[0x35] = {236, 174, 212, 255};  // Light peach
        nesColors_[0x36] = {236, 180, 176, 255};  // Cream
        nesColors_[0x37] = {228, 196, 144, 255};  // Light yellow
        nesColors_[0x38] = {204, 210, 120, 255};  // Pale yellow
        nesColors_[0x39] = {180, 222, 120, 255};  // Pale yellow-green
        nesColors_[0x3A] = {168, 226, 144, 255};  // Pale green
        nesColors_[0x3B] = {152, 226, 180, 255};  // Pale cyan-green
        nesColors_[0x3C] = {160, 214, 228, 255};  // Pale cyan
        nesColors_[0x3D] = {160, 162, 160, 255};  // Light gray
        nesColors_[0x3E] = {0, 0, 0, 255};        // Black
        nesColors_[0x3F] = {0, 0, 0, 255};        // Black
    }
};

// Battle City specific color usage
class BattleCityPalette {
public:
    static constexpr uint8_t COLOR_BLACK = 0x0D;
    static constexpr uint8_t COLOR_WHITE = 0x20;
    static constexpr uint8_t COLOR_RED = 0x06;      // Player tank red
    static constexpr uint8_t COLOR_BLUE = 0x02;     // Player 2 tank blue
    static constexpr uint8_t COLOR_YELLOW = 0x28;   // Brick wall
    static constexpr uint8_t COLOR_GRAY = 0x00;     // Basic enemy
    static constexpr uint8_t COLOR_GREEN = 0x0A;    // Grass
    static constexpr uint8_t COLOR_CYAN = 0x1C;     // Water
    static constexpr uint8_t COLOR_ORANGE = 0x16;   // Explosions
    static constexpr uint8_t COLOR_PINK = 0x24;     // Timer bomb
};

} // namespace BattleCity