#pragma once

#include <SDL.h>
#include <memory>
#include "Palette.h"
#include "../utils/MathUtils.h"
#include "../gameplay/PowerUp.h"

namespace BattleCity {

// Pixel-perfect renderer for NES-style graphics
class Renderer {
private:
    SDL_Window* window_;
    SDL_Renderer* renderer_;
    SDL_Texture* gameTexture_;  // 256x224 game texture
    std::unique_ptr<Palette> palette_;

    int scaleFactor_;
    bool vsyncEnabled_;
    static constexpr int GAME_WIDTH = 256;
    static constexpr int GAME_HEIGHT = 224;

public:
    Renderer(int scaleFactor = 3, bool vsync = true);
    ~Renderer();

    // Initialize SDL and create window/renderer
    bool init();

    // Clear screen
    void clear();

    // Present rendered frame
    void present();

    // Pixel rendering functions
    void setPixel(int x, int y, uint8_t colorIndex);
    uint8_t getPixel(int x, int y) const;

    // Rectangle rendering
    void fillRect(int x, int y, int w, int h, uint8_t colorIndex);
    void drawRect(int x, int y, int w, int h, uint8_t colorIndex);

    // Sprite rendering (8x8 pixels)
    void drawSprite(int x, int y, const uint8_t* spriteData, uint8_t colorIndex = 0x20);

    // Text rendering
    void drawText(int x, int y, const char* text, uint8_t colorIndex = 0x20);

    // Special effects
    void drawExplosion(int x, int y, int frame);
    void drawShield(int x, int y, int frame);
    void drawPowerUpIcon(int x, int y, PowerUpType type);

    // Screen effects
    void fadeIn(float alpha);
    void fadeOut(float alpha);

    // Getters
    int getWidth() const { return GAME_WIDTH; }
    int getHeight() const { return GAME_HEIGHT; }
    int getScaleFactor() const { return scaleFactor_; }
    SDL_Window* getWindow() const { return window_; }

private:
    // Internal rendering helpers
    void updateGameTexture();
    void renderScaled();

    // Font data (8x8 pixel font)
    static const uint8_t FONT_DATA[128][8];
};

// Sprite data structure
struct Sprite {
    static constexpr int WIDTH = 8;
    static constexpr int HEIGHT = 8;
    uint8_t pixels[WIDTH * HEIGHT];

    Sprite() {
        memset(pixels, 0, sizeof(pixels));
    }

    void setPixel(int x, int y, uint8_t color) {
        if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
            pixels[y * WIDTH + x] = color;
        }
    }

    uint8_t getPixel(int x, int y) const {
        if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
            return pixels[y * WIDTH + x];
        }
        return 0;
    }
};

// Tank sprites (NES-style 8x8 pixels)
class TankSprites {
public:
    static const Sprite PLAYER_TANK_UP[4];     // 4 animation frames
    static const Sprite PLAYER_TANK_DOWN[4];
    static const Sprite PLAYER_TANK_LEFT[4];
    static const Sprite PLAYER_TANK_RIGHT[4];

    static const Sprite ENEMY_TANK_BASIC_UP[2];
    static const Sprite ENEMY_TANK_BASIC_DOWN[2];
    static const Sprite ENEMY_TANK_BASIC_LEFT[2];
    static const Sprite ENEMY_TANK_BASIC_RIGHT[2];

    static const Sprite ENEMY_TANK_FAST_UP[2];
    static const Sprite ENEMY_TANK_FAST_DOWN[2];
    static const Sprite ENEMY_TANK_FAST_LEFT[2];
    static const Sprite ENEMY_TANK_FAST_RIGHT[2];

    static const Sprite ENEMY_TANK_HEAVY_UP[2];
    static const Sprite ENEMY_TANK_HEAVY_DOWN[2];
    static const Sprite ENEMY_TANK_HEAVY_LEFT[2];
    static const Sprite ENEMY_TANK_HEAVY_RIGHT[2];

    static const Sprite ENEMY_TANK_ELITE_UP[2];
    static const Sprite ENEMY_TANK_ELITE_DOWN[2];
    static const Sprite ENEMY_TANK_ELITE_LEFT[2];
    static const Sprite ENEMY_TANK_ELITE_RIGHT[2];

    static const Sprite BULLET_UP;
    static const Sprite BULLET_DOWN;
    static const Sprite BULLET_LEFT;
    static const Sprite BULLET_RIGHT;

    static const Sprite EXPLOSION[3];  // 3 frames
    static const Sprite BASE;
    static const Sprite BASE_DESTROYED;
};

} // namespace BattleCity