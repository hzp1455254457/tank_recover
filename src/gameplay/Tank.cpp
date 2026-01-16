#include "Tank.h"
#include "../core/Game.h"
#include <algorithm>

namespace BattleCity {

Tank::Tank(Game* game)
    : position_(0, 0), velocity_(0, 0), direction_(Direction::UP), lastDirection_(Direction::UP),
      health_(1), maxHealth_(1), level_(0), invincible_(false), invincibleTimer_(0),
      animationFrame_(0), animationTimer_(0), isActive_(true), canMove_(true), canShoot_(true),
      moveSpeed_(0), shootCooldown_(0), currentCooldown_(0), bulletSpeed_(0), bulletPower_(0),
      game_(game) {
}

void Tank::move() {
    if (!canMove_ || !isActive_) return;

    // Calculate new position
    Vector2 newPos = position_ + velocity_;

    // Check bounds and collision
    if (canMoveTo(newPos)) {
        position_ = newPos;

        // Update animation
        updateAnimation();

        // Update last direction if actually moved
        if (velocity_.x != 0 || velocity_.y != 0) {
            lastDirection_ = direction_;
        }
    }
}

void Tank::setDirection(Direction dir) {
    direction_ = dir;
    velocity_ = MathUtils::directionToVelocity(dir, getMoveSpeed());
}

void Tank::stop() {
    velocity_ = Vector2(0, 0);
}

void Tank::takeDamage(int damage) {
    if (!isActive_ || invincible_) return;

    health_ -= damage;
    if (health_ <= 0) {
        health_ = 0;
        isActive_ = false;
    } else {
        // Activate brief invincibility after taking damage
        activateInvincibility(20); // 20 frames
    }
}

void Tank::heal(int amount) {
    health_ = std::min(maxHealth_, health_ + amount);
}

void Tank::activateInvincibility(int frames) {
    invincible_ = true;
    invincibleTimer_ = frames;
}

void Tank::upgrade() {
    if (level_ < 3) {
        level_++;
        // Update stats based on new level
        moveSpeed_ = getMoveSpeed();
        shootCooldown_ = getShootCooldown();
        bulletSpeed_ = getBulletSpeed();
        bulletPower_ = getBulletPower();
    }
}

Rect Tank::getBounds() const {
    return Rect(position_.pixelX(), position_.pixelY(), 8, 8);
}

bool Tank::collidesWith(const Tank& other) const {
    if (!isActive_ || !other.isActive_) return false;

    Rect myBounds = getBounds();
    Rect otherBounds = other.getBounds();

    return myBounds.intersects(otherBounds);
}

void Tank::updateAnimation() {
    animationTimer_++;
    if (animationTimer_ >= 10) { // 10 frames per animation frame at 60fps
        animationFrame_ = (animationFrame_ + 1) % 2; // 2-frame animation
        animationTimer_ = 0;
    }
}

void Tank::updateInvincibility() {
    if (invincible_) {
        invincibleTimer_--;
        if (invincibleTimer_ <= 0) {
            invincible_ = false;
        }
    }
}

void Tank::shoot() {
    if (!canShoot_ || currentCooldown_ > 0 || !isActive_ || !game_) return;

    // Calculate bullet spawn position (front of tank)
    Vector2 bulletPos = position_;
    switch (direction_) {
        case Direction::UP:    bulletPos.y -= 4 * 256; break; // 4 pixels up
        case Direction::DOWN:  bulletPos.y += 4 * 256; break; // 4 pixels down
        case Direction::LEFT:  bulletPos.x -= 4 * 256; break; // 4 pixels left
        case Direction::RIGHT: bulletPos.x += 4 * 256; break; // 4 pixels right
        default: break;
    }

    // Create bullet through game
    game_->shootBullet(bulletPos, direction_, getBulletOwner(), bulletPower_);

    // Set cooldown
    currentCooldown_ = shootCooldown_;
}

void Tank::updateCooldown() {
    if (currentCooldown_ > 0) {
        currentCooldown_--;
    }
}

bool Tank::canMoveTo(const Vector2& newPos) const {
    // Check screen bounds
    int pixelX = newPos.pixelX();
    int pixelY = newPos.pixelY();

    if (pixelX < 0 || pixelX > 256 - 8 || pixelY < 0 || pixelY > 224 - 8) {
        return false;
    }

    // Check terrain collision (would be implemented with terrain system)
    // For now, allow all movement
    return true;
}

} // namespace BattleCity