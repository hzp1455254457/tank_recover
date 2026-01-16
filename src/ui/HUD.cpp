#include "HUD.h"
#include "../graphics/Renderer.h"
#include <sstream>
#include <iomanip>
#include <iostream>

namespace BattleCity {

class Game; // Forward declaration to avoid circular dependency

void HUD::render(Renderer& renderer, int score, int lives, int level, bool isTwoPlayerMode) {
    std::cout << "HUD::render score=" << score << " lives=" << lives << " level=" << level << " twoPlayer=" << isTwoPlayerMode << std::endl;
    // Render player 1 lives
    renderPlayerLives(renderer, lives, PLAYER1_LIFE_X, PLAYER1_LIFE_Y);

    // Render player 2 lives (if in 2-player mode)
    if (isTwoPlayerMode) {
        renderPlayerLives(renderer, lives, PLAYER2_LIFE_X, PLAYER2_LIFE_Y);
    }

    // Render score
    renderScore(renderer, score, SCORE_X, SCORE_Y);

    // Render current level
    renderLevel(renderer, level, LEVEL_TEXT_X, LEVEL_TEXT_Y);
}

void HUD::renderPlayerLives(Renderer& renderer, int lives, int x, int y) {
    std::cout << "HUD::renderPlayerLives rendering " << lives << " lives at (" << x << "," << y << ")" << std::endl;
    // Render life icons as small rectangles
    for (int i = 0; i < lives; ++i) {
        int iconX = x + i * (LIFE_ICON_SIZE + 2);
        std::cout << "  Rendering life icon " << i << " at (" << iconX << "," << y << ")" << std::endl;
        renderer.fillRect(iconX, y, LIFE_ICON_SIZE, LIFE_ICON_SIZE, BattleCityPalette::COLOR_RED);
    }
}

void HUD::renderScore(Renderer& renderer, int score, int x, int y) {
    std::string scoreStr = formatScore(score);
    std::cout << "HUD::renderScore rendering score '" << scoreStr << "' at (" << x << "," << y << ")" << std::endl;
    renderer.drawText(x, y, scoreStr.c_str());
}

void HUD::renderLevel(Renderer& renderer, int level, int x, int y) {
    std::string levelStr = "STAGE " + formatLevel(level);
    std::cout << "HUD::renderLevel rendering level '" << levelStr << "' at (" << x << "," << y << ")" << std::endl;
    renderer.drawText(x, y, levelStr.c_str());
}

void HUD::renderPowerUpIcon(PowerUpType type, int x, int y) {
    const uint8_t* iconSprite = getPowerUpIconSprite(type);

    // TODO: Render actual sprite when graphics system is complete
    // For now, placeholder
}

std::string HUD::formatScore(int score) const {
    std::stringstream ss;
    ss << std::setw(SCORE_DIGITS) << std::setfill('0') << score;
    return ss.str();
}

std::string HUD::formatLevel(int level) const {
    std::stringstream ss;
    ss << std::setw(LEVEL_DIGITS) << std::setfill('0') << level;
    return ss.str();
}

const uint8_t* HUD::getLifeIconSprite() const {
    // Tank life icon sprite (8x8)
    static const uint8_t lifeIcon[8] = {
        0x0F, 0x0F, 0x0F, 0x0F,
        0x0F, 0x00, 0x00, 0x0F
    };
    return lifeIcon;
}

const uint8_t* HUD::getPowerUpIconSprite(PowerUpType type) const {
    // Power-up icon sprites (8x8 each)
    static const uint8_t starIcon[8] = {
        0x00, 0x04, 0x00, 0x00,
        0x04, 0x0E, 0x04, 0x00
    };

    static const uint8_t tankIcon[8] = {
        0x0F, 0x09, 0x09, 0x0F,
        0x00, 0x00, 0x00, 0x00
    };

    static const uint8_t clockIcon[8] = {
        0x0E, 0x0A, 0x0A, 0x0E,
        0x00, 0x04, 0x00, 0x00
    };

    static const uint8_t shieldIcon[8] = {
        0x04, 0x0E, 0x1F, 0x0E,
        0x04, 0x00, 0x00, 0x00
    };

    static const uint8_t bombIcon[8] = {
        0x04, 0x0E, 0x1B, 0x0E,
        0x04, 0x00, 0x00, 0x00
    };

    switch (type) {
        case PowerUpType::TANK_UPGRADE: return starIcon;
        case PowerUpType::EXTRA_LIFE:   return tankIcon;
        case PowerUpType::TIMER_BOMB:   return clockIcon;
        case PowerUpType::SHIELD:       return shieldIcon;
        case PowerUpType::CLEAR_ENEMIES: return bombIcon;
        default: return starIcon;
    }
}

} // namespace BattleCity