#include "Renderer.h"
#include <cstring>
#include <stdexcept>
#include <iostream>

namespace BattleCity {

Renderer::Renderer(int scaleFactor, bool vsync)
    : window_(nullptr), renderer_(nullptr), gameTexture_(nullptr),
      scaleFactor_(scaleFactor), vsyncEnabled_(vsync) {
    palette_ = std::make_unique<Palette>();
}

Renderer::~Renderer() {
    if (gameTexture_) SDL_DestroyTexture(gameTexture_);
    if (renderer_) SDL_DestroyRenderer(renderer_);
    if (window_) SDL_DestroyWindow(window_);
    SDL_Quit();
}

bool Renderer::init() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        return false;
    }

    // Create window
    int windowWidth = GAME_WIDTH * scaleFactor_;
    int windowHeight = GAME_HEIGHT * scaleFactor_;
    window_ = SDL_CreateWindow("Battle City - PC Remake",
                              SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              windowWidth, windowHeight,
                              SDL_WINDOW_SHOWN);
    if (!window_) return false;

    // Create renderer with VSync
    Uint32 renderFlags = SDL_RENDERER_ACCELERATED;
    if (vsyncEnabled_) {
        renderFlags |= SDL_RENDERER_PRESENTVSYNC;
    }
    renderer_ = SDL_CreateRenderer(window_, -1, renderFlags);
    if (!renderer_) return false;

    // Create game texture (256x224)
    gameTexture_ = SDL_CreateTexture(renderer_, SDL_PIXELFORMAT_RGBA8888,
                                   SDL_TEXTUREACCESS_STREAMING,
                                   GAME_WIDTH, GAME_HEIGHT);
    if (!gameTexture_) return false;

    // Set logical size for pixel-perfect rendering
    SDL_RenderSetLogicalSize(renderer_, GAME_WIDTH, GAME_HEIGHT);

    return true;
}

void Renderer::clear() {
    SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 255);
    SDL_RenderClear(renderer_);
}

void Renderer::present() {
    std::cout << "Renderer::present() called" << std::endl;
    updateGameTexture();
    renderScaled();
    SDL_RenderPresent(renderer_);
}

void Renderer::setPixel(int x, int y, uint8_t colorIndex) {
    if (x < 0 || x >= GAME_WIDTH || y < 0 || y >= GAME_HEIGHT) return;

    // Get NES color
    const SDL_Color& color = palette_->getColor(colorIndex);

    // Set render color
    SDL_SetRenderDrawColor(renderer_, color.r, color.g, color.b, color.a);

    // Draw pixel
    SDL_RenderDrawPoint(renderer_, x, y);
}

uint8_t Renderer::getPixel(int x, int y) const {
    // This would require reading back from the texture, which is expensive
    // For now, return 0 (black)
    return 0;
}

void Renderer::fillRect(int x, int y, int w, int h, uint8_t colorIndex) {
    const SDL_Color& color = palette_->getColor(colorIndex);
    SDL_SetRenderDrawColor(renderer_, color.r, color.g, color.b, color.a);

    SDL_Rect rect = {x, y, w, h};
    SDL_RenderFillRect(renderer_, &rect);
}

void Renderer::drawRect(int x, int y, int w, int h, uint8_t colorIndex) {
    const SDL_Color& color = palette_->getColor(colorIndex);
    SDL_SetRenderDrawColor(renderer_, color.r, color.g, color.b, color.a);

    SDL_Rect rect = {x, y, w, h};
    SDL_RenderDrawRect(renderer_, &rect);
}

void Renderer::drawSprite(int x, int y, const uint8_t* spriteData, uint8_t colorIndex) {
    for (int sy = 0; sy < 8; ++sy) {
        for (int sx = 0; sx < 8; ++sx) {
            uint8_t pixel = spriteData[sy * 8 + sx];
            if (pixel != 0) {  // 0 = transparent
                setPixel(x + sx, y + sy, pixel);
            }
        }
    }
}

void Renderer::drawText(int x, int y, const char* text, uint8_t colorIndex) {
    if (text == nullptr) return;
    std::cout << "Renderer::drawText(\"" << text << "\") at (" << x << "," << y << ") color=" << static_cast<int>(colorIndex) << std::endl;
    int currentX = x;
    while (*text) {
        if (*text >= 32 && *text < 128) {
            const uint8_t* charData = FONT_DATA[*text - 32];
            for (int cy = 0; cy < 8; ++cy) {
                for (int cx = 0; cx < 8; ++cx) {
                    if (charData[cy] & (1 << (7 - cx))) {
                        setPixel(currentX + cx, y + cy, colorIndex);
                    }
                }
            }
        }
        currentX += 8;
        ++text;
    }
}

void Renderer::drawExplosion(int x, int y, int frame) {
    // Explosion animation frames
    static const uint8_t explosionData[3][64] = {
        // Frame 0: Small explosion
        {
            0,0,0,6,6,0,0,0,
            0,0,6,6,6,6,0,0,
            0,6,6,7,7,6,6,0,
            0,6,7,7,7,7,6,0,
            0,6,7,7,7,7,6,0,
            0,6,6,7,7,6,6,0,
            0,0,6,6,6,6,0,0,
            0,0,0,6,6,0,0,0
        },
        // Frame 1: Medium explosion
        {
            0,6,6,7,7,6,6,0,
            6,7,7,7,7,7,7,6,
            6,7,7,8,8,7,7,6,
            6,7,8,8,8,8,7,6,
            6,7,8,8,8,8,7,6,
            6,7,7,8,8,7,7,6,
            6,7,7,7,7,7,7,6,
            0,6,6,7,7,6,6,0
        },
        // Frame 2: Large explosion
        {
            6,7,7,8,8,7,7,6,
            7,8,8,9,9,8,8,7,
            7,8,9,9,9,9,8,7,
            7,8,9,9,9,9,8,7,
            7,8,9,9,9,9,8,7,
            7,8,9,9,9,9,8,7,
            7,8,8,9,9,8,8,7,
            6,7,7,8,8,7,7,6
        }
    };

    if (frame >= 0 && frame < 3) {
        drawSprite(x - 4, y - 4, explosionData[frame]);
    }
}

void Renderer::drawShield(int x, int y, int frame) {
    // Shield blinking effect
    uint8_t color = (frame % 2 == 0) ? BattleCityPalette::COLOR_WHITE : BattleCityPalette::COLOR_CYAN;

    // Draw shield outline
    drawRect(x - 1, y - 1, 10, 10, color);
}

void Renderer::drawPowerUpIcon(int x, int y, PowerUpType type) {
    // Simple icon representation - would be replaced with actual sprites
    uint8_t color = BattleCityPalette::COLOR_YELLOW; // Default

    switch (type) {
        case PowerUpType::TANK_UPGRADE: color = BattleCityPalette::COLOR_YELLOW; break;
        case PowerUpType::EXTRA_LIFE: color = BattleCityPalette::COLOR_GREEN; break;
        case PowerUpType::TIMER_BOMB: color = BattleCityPalette::COLOR_PINK; break;
        case PowerUpType::SHIELD: color = BattleCityPalette::COLOR_CYAN; break;
        case PowerUpType::CLEAR_ENEMIES: color = BattleCityPalette::COLOR_WHITE; break;
    }

    fillRect(x, y, 8, 8, color);
}

void Renderer::fadeIn(float alpha) {
    // Draw semi-transparent black overlay
    uint8_t alphaByte = static_cast<uint8_t>((1.0f - alpha) * 255);
    SDL_SetRenderDrawColor(renderer_, 0, 0, 0, alphaByte);
    SDL_Rect rect = {0, 0, GAME_WIDTH, GAME_HEIGHT};
    SDL_RenderFillRect(renderer_, &rect);
}

void Renderer::fadeOut(float alpha) {
    // Draw semi-transparent black overlay
    uint8_t alphaByte = static_cast<uint8_t>(alpha * 255);
    SDL_SetRenderDrawColor(renderer_, 0, 0, 0, alphaByte);
    SDL_Rect rect = {0, 0, GAME_WIDTH, GAME_HEIGHT};
    SDL_RenderFillRect(renderer_, &rect);
}

void Renderer::updateGameTexture() {
    // This would copy the rendered pixels to the game texture
    // For now, we'll just render directly
}

void Renderer::renderScaled() {
    // The game texture is already rendered with logical size.
    // Do NOT clear the renderer here — clearing here erases any pixels
    // drawn earlier by setPixel/drawText/drawSprite. SDL_RenderSetLogicalSize
    // will handle scaling when presenting.
    // (Keeping this function minimal so present() doesn't erase drawings.)
}

// Font data (8x8 pixel monospace font, NES style)
const uint8_t Renderer::FONT_DATA[128][8] = {
    // Space (32)
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    // Numbers 0-9 (48-57)
    {0x3C, 0x42, 0x42, 0x42, 0x42, 0x42, 0x3C, 0x00}, // 0
    {0x08, 0x18, 0x08, 0x08, 0x08, 0x08, 0x1C, 0x00}, // 1
    {0x3C, 0x42, 0x02, 0x3C, 0x40, 0x40, 0x7E, 0x00}, // 2
    {0x3C, 0x42, 0x02, 0x1C, 0x02, 0x42, 0x3C, 0x00}, // 3
    {0x04, 0x0C, 0x14, 0x24, 0x7E, 0x04, 0x04, 0x00}, // 4
    {0x7E, 0x40, 0x7C, 0x02, 0x02, 0x42, 0x3C, 0x00}, // 5
    {0x3C, 0x40, 0x7C, 0x42, 0x42, 0x42, 0x3C, 0x00}, // 6
    {0x7E, 0x02, 0x04, 0x08, 0x10, 0x10, 0x10, 0x00}, // 7
    {0x3C, 0x42, 0x42, 0x3C, 0x42, 0x42, 0x3C, 0x00}, // 8
    {0x3C, 0x42, 0x42, 0x3E, 0x02, 0x42, 0x3C, 0x00}, // 9
    // Uppercase A-Z (65-90)
    {0x18, 0x24, 0x42, 0x42, 0x7E, 0x42, 0x42, 0x00}, // A
    {0x7C, 0x42, 0x42, 0x7C, 0x42, 0x42, 0x7C, 0x00}, // B
    {0x3C, 0x42, 0x40, 0x40, 0x40, 0x42, 0x3C, 0x00}, // C
    {0x78, 0x44, 0x42, 0x42, 0x42, 0x44, 0x78, 0x00}, // D
    {0x7E, 0x40, 0x40, 0x7C, 0x40, 0x40, 0x7E, 0x00}, // E
    {0x7E, 0x40, 0x40, 0x7C, 0x40, 0x40, 0x40, 0x00}, // F
    {0x3C, 0x42, 0x40, 0x4E, 0x42, 0x42, 0x3C, 0x00}, // G
    {0x42, 0x42, 0x42, 0x7E, 0x42, 0x42, 0x42, 0x00}, // H
    {0x1C, 0x08, 0x08, 0x08, 0x08, 0x08, 0x1C, 0x00}, // I
    {0x02, 0x02, 0x02, 0x02, 0x42, 0x42, 0x3C, 0x00}, // J
    {0x42, 0x44, 0x48, 0x70, 0x48, 0x44, 0x42, 0x00}, // K
    {0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x7E, 0x00}, // L
    {0x42, 0x66, 0x5A, 0x42, 0x42, 0x42, 0x42, 0x00}, // M
    {0x42, 0x62, 0x52, 0x4A, 0x46, 0x42, 0x42, 0x00}, // N
    {0x3C, 0x42, 0x42, 0x42, 0x42, 0x42, 0x3C, 0x00}, // O
    {0x7C, 0x42, 0x42, 0x7C, 0x40, 0x40, 0x40, 0x00}, // P
    {0x3C, 0x42, 0x42, 0x42, 0x4A, 0x44, 0x3A, 0x00}, // Q
    {0x7C, 0x42, 0x42, 0x7C, 0x48, 0x44, 0x42, 0x00}, // R
    {0x3C, 0x42, 0x40, 0x3C, 0x02, 0x42, 0x3C, 0x00}, // S
    {0x7E, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x00}, // T
    {0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x3C, 0x00}, // U
    {0x42, 0x42, 0x42, 0x42, 0x42, 0x24, 0x18, 0x00}, // V
    {0x42, 0x42, 0x42, 0x42, 0x5A, 0x66, 0x42, 0x00}, // W
    {0x42, 0x24, 0x18, 0x18, 0x18, 0x24, 0x42, 0x00}, // X
    {0x42, 0x42, 0x24, 0x18, 0x18, 0x18, 0x18, 0x00}, // Y
    {0x7E, 0x02, 0x04, 0x18, 0x20, 0x40, 0x7E, 0x00}, // Z
};

} // namespace BattleCity