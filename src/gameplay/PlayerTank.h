#pragma once

#include "Tank.h"
#include "../input/InputManager.h"

namespace BattleCity {

class PlayerTank : public Tank {
private:
    int playerIndex_;           // 0 for player 1, 1 for player 2
    int lives_;                 // Remaining lives
    int score_;                 // Current score
    int tankLevel_;             // Tank upgrade level (0-3)
    bool hasShield_;            // Shield power-up status
    int shieldTimer_;           // Shield duration

public:
    PlayerTank(int playerIndex = 0, Game* game = nullptr);

    void update() override;
    void render(Renderer& renderer) override;
    void shoot() override;

    // Player-specific methods
    void handleInput(const InputManager& input);
    void addScore(int points);
    void loseLife();
    void gainLife();
    bool isGameOver() const;

    // Score calculation
    int calculateEnemyScore(EnemyType enemyType) const;

    // Getters
    int getPlayerIndex() const { return playerIndex_; }
    int getLives() const { return lives_; }
    int getScore() const { return score_; }
    bool hasShield() const { return hasShield_; }
    Vector2 getSpawnPosition() const;
    
    // Setters for initialization
    void setLives(int lives) { lives_ = lives; }
    void setScore(int score) { score_ = score; }

    // Power-ups
    void activateShield(int frames);
    void deactivateShield();

protected:
    int getMoveSpeed() const override;
    int getShootCooldown() const override;
    int getBulletSpeed() const override;
    int getBulletPower() const override;
    BulletOwner getBulletOwner() const override;

private:
    void updateShield();
    void renderShield(Renderer& renderer) const;
    const uint8_t* getSpriteData() const;
};

} // namespace BattleCity