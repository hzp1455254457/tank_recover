#pragma once

#include "../utils/MathUtils.h"
#include "../graphics/Renderer.h"
#include "../gameplay/Bullet.h"
#include <memory>

namespace BattleCity {

class Bullet;
class LevelManager;
class Game;

// Tank base class
class Tank {
protected:
    Vector2 position_;           // Sub-pixel position
    Vector2 velocity_;           // Current movement velocity
    Direction direction_;        // Current facing direction
    Direction lastDirection_;    // Last movement direction

    int health_;                 // Current health points
    int maxHealth_;              // Maximum health points
    int level_;                  // Upgrade level (0-3)
    bool invincible_;            // Invincibility status
    int invincibleTimer_;        // Invincibility duration in frames

    int animationFrame_;         // Current animation frame
    int animationTimer_;         // Animation timer

    bool isActive_;              // Whether tank is active
    bool canMove_;               // Movement permission
    bool canShoot_;              // Shooting permission

    // Combat stats
    int moveSpeed_;              // Movement speed (sub-pixel units)
    int shootCooldown_;          // Frames between shots
    int currentCooldown_;        // Current cooldown counter
    int bulletSpeed_;            // Bullet speed
    int bulletPower_;            // Bullet penetration power

    // Game reference for bullet creation
    Game* game_;                 // Reference to game for bullet management

public:
    Tank(Game* game = nullptr);
    virtual ~Tank() = default;

    // Core methods
    virtual void update() = 0;
    virtual void render(Renderer& renderer) = 0;
    virtual void shoot() = 0;
    virtual void destroy() { isActive_ = false; }

    // Movement
    void move();
    void setDirection(Direction dir);
    Direction getDirection() const { return direction_; }
    void stop();

    // Combat
    void takeDamage(int damage);
    void heal(int amount);
    void activateInvincibility(int frames);
    bool isInvincible() const { return invincible_; }

    // Upgrades
    void upgrade();
    int getLevel() const { return level_; }

    // State queries
    bool isActive() const { return isActive_; }
    bool isAlive() const { return health_ > 0; }
    int getHealth() const { return health_; }
    Vector2 getPosition() const { return position_; }
    void setPosition(const Vector2& pos) { position_ = pos; }

    // Collision
    virtual Rect getBounds() const;
    bool collidesWith(const Tank& other) const;

protected:
    // Virtual methods for derived classes
    virtual void updateAnimation();
    virtual int getMoveSpeed() const = 0;
    virtual int getShootCooldown() const = 0;
    virtual int getBulletSpeed() const = 0;
    virtual int getBulletPower() const = 0;
    virtual BulletOwner getBulletOwner() const = 0;

    // Helper methods
    void updateInvincibility();
    void updateCooldown();
    bool canMoveTo(const Vector2& newPos) const;
};

} // namespace BattleCity