#pragma once

#include "../utils/MathUtils.h"

namespace BattleCity {

enum class BulletOwner {
    PLAYER_1,
    PLAYER_2,
    ENEMY
};

class Bullet {
private:
    Vector2 position_;           // Sub-pixel position
    Direction direction_;        // Movement direction
    BulletOwner owner_;          // Who fired this bullet
    int speed_;                  // Movement speed (sub-pixel units)
    int power_;                  // Penetration power (1-3)
    bool isActive_;              // Whether bullet is active
    int lifetime_;               // Frames until auto-destroy
    int maxLifetime_;            // Maximum lifetime

    // Animation
    int animationFrame_;
    int animationTimer_;

public:
    Bullet();

    // Initialization
    void init(const Vector2& position, Direction direction, BulletOwner owner, int power = 1);

    // Update bullet logic
    void update();

    // Collision detection
    bool checkCollisionWithTank(class Tank& tank);
    bool checkCollisionWithTerrain(class LevelManager& levelManager);
    bool checkCollisionWithBase(const Vector2& basePos);

    // Getters
    Vector2 getPosition() const { return position_; }
    Direction getDirection() const { return direction_; }
    BulletOwner getOwner() const { return owner_; }
    int getPower() const { return power_; }
    bool isActive() const { return isActive_; }
    Rect getBounds() const;

    // State management
    void deactivate();
    void setLifetime(int frames);

    // Rendering
    void render(class Renderer& renderer) const;

private:
    // Movement
    void move();

    // Collision helpers
    bool isValidPosition(const Vector2& pos, class LevelManager& levelManager) const;
    bool canPenetrate(TerrainType terrain) const;

    // Animation
    void updateAnimation();
    const uint8_t* getSpriteData() const;
};

} // namespace BattleCity