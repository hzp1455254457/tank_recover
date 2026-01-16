#pragma once

#include "../gameplay/PowerUp.h"
#include "../graphics/Renderer.h"
#include <string>

namespace BattleCity {

class Game; // Forward declaration to avoid circular dependency

class HUD {
private:
    static constexpr int LIFE_ICON_SIZE = 8;
    static constexpr int SCORE_DIGITS = 6;
    static constexpr int LEVEL_DIGITS = 2;

    // HUD layout positions
    static constexpr int PLAYER1_LIFE_X = 8;
    static constexpr int PLAYER1_LIFE_Y = 8;
    static constexpr int PLAYER2_LIFE_X = 80;
    static constexpr int PLAYER2_LIFE_Y = 8;

    static constexpr int SCORE_X = 40;
    static constexpr int SCORE_Y = 16;

    static constexpr int LEVEL_TEXT_X = 180;
    static constexpr int LEVEL_TEXT_Y = 8;

public:
    void render(Renderer& renderer, int score, int lives, int level, bool isTwoPlayerMode = false);

private:
    void renderPlayerLives(Renderer& renderer, int lives, int x, int y);
    void renderScore(Renderer& renderer, int score, int x, int y);
    void renderLevel(Renderer& renderer, int level, int x, int y);
    void renderPowerUpIcon(PowerUpType type, int x, int y);

    // Helper methods
    std::string formatScore(int score) const;
    std::string formatLevel(int level) const;
    const uint8_t* getLifeIconSprite() const;
    const uint8_t* getPowerUpIconSprite(PowerUpType type) const;
};

} // namespace BattleCity