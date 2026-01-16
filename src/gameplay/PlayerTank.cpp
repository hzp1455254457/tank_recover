#include "PlayerTank.h"

namespace BattleCity {

PlayerTank::PlayerTank(int playerIndex, Game* game)
    : Tank(game), playerIndex_(playerIndex), lives_(3), score_(0),
      hasShield_(false), shieldTimer_(0) {
    // Initialize tank properties
    maxHealth_ = 1;
    health_ = 1;
    level_ = 0;

    // Set initial stats
    moveSpeed_ = getMoveSpeed();
    shootCooldown_ = getShootCooldown();
    bulletSpeed_ = getBulletSpeed();
    bulletPower_ = getBulletPower();
}

void PlayerTank::update() {
    updateInvincibility();
    updateCooldown();
    updateShield();

    // Update animation
    if (velocity_.x != 0 || velocity_.y != 0) {
        updateAnimation();
    } else {
        animationFrame_ = 0; // Reset to first frame when stopped
    }
}

void PlayerTank::render(Renderer& renderer) {
    if (!isActive_) return;

    int x = position_.pixelX();
    int y = position_.pixelY();

    // Render tank sprite
    const uint8_t* spriteData = getSpriteData();
    renderer.drawSprite(x, y, spriteData);

    // Render shield effect if active
    if (hasShield_) {
        renderShield(renderer);
    }
}

void PlayerTank::shoot() {
    if (!canShoot_ || currentCooldown_ > 0 || !isActive_) return;

    // Create bullet (would be handled by bullet manager)
    // BulletManager::createBullet(this, direction_, bulletSpeed_, bulletPower_);

    currentCooldown_ = shootCooldown_;
}

void PlayerTank::handleInput(const InputManager& input) {
    if (!isActive_) return;

    Direction newDirection = Direction::NONE;
    bool shouldShoot = false;

    // Check movement inputs
    if (input.isPressed(GameAction::UP, playerIndex_)) {
        newDirection = Direction::UP;
    } else if (input.isPressed(GameAction::DOWN, playerIndex_)) {
        newDirection = Direction::DOWN;
    } else if (input.isPressed(GameAction::LEFT, playerIndex_)) {
        newDirection = Direction::LEFT;
    } else if (input.isPressed(GameAction::RIGHT, playerIndex_)) {
        newDirection = Direction::RIGHT;
    }

    // Handle movement
    if (newDirection != Direction::NONE) {
        setDirection(newDirection);
        move();
    } else {
        stop();
    }

    // Handle shooting
    if (input.isPressed(GameAction::SHOOT, playerIndex_)) {
        shoot();
    }
}

void PlayerTank::addScore(int points) {
    score_ += points;
}

int PlayerTank::calculateEnemyScore(EnemyType enemyType) const {
    // Base scores for enemy types
    int baseScore;
    switch (enemyType) {
        case EnemyType::BASIC: baseScore = 100; break;
        case EnemyType::FAST:  baseScore = 200; break;
        case EnemyType::HEAVY: baseScore = 300; break;
        case EnemyType::ELITE: baseScore = 400; break;
        default: baseScore = 100; break;
    }

    // Level multiplier (tank upgrade level + 1)
    int levelMultiplier = tankLevel_ + 1;

    return baseScore * levelMultiplier;
}

void PlayerTank::loseLife() {
    lives_--;
    if (lives_ <= 0) {
        isActive_ = false;
    } else {
        // Reset tank for respawn
        health_ = maxHealth_;
        level_ = 0;
        invincible_ = false;
        hasShield_ = false;
        position_ = getSpawnPosition();
    }
}

void PlayerTank::gainLife() {
    lives_++;
}

bool PlayerTank::isGameOver() const {
    return lives_ <= 0;
}

void PlayerTank::activateShield(int frames) {
    hasShield_ = true;
    shieldTimer_ = frames;
    invincible_ = true; // Shield provides invincibility
}

void PlayerTank::deactivateShield() {
    hasShield_ = false;
    shieldTimer_ = 0;
    invincible_ = false;
}

int PlayerTank::getMoveSpeed() const {
    // Base speed: 1 pixel/frame, upgrades don't affect speed
    return 256; // 1 pixel in sub-pixel units
}

int PlayerTank::getShootCooldown() const {
    // Base cooldown: 15 frames, reduced with upgrades
    int baseCooldown = 15;
    switch (level_) {
        case 1: return baseCooldown - 3;  // Faster shooting
        case 2: return baseCooldown - 6;  // Even faster
        case 3: return baseCooldown - 9;  // Fastest shooting
        default: return baseCooldown;
    }
}

int PlayerTank::getBulletSpeed() const {
    // Base speed: 3 pixels/frame, increased with level 2+
    int baseSpeed = 3;
    return (level_ >= 2) ? baseSpeed + 1 : baseSpeed;
}

int PlayerTank::getBulletPower() const {
    // Base power: 1, increases with level (steel wall penetration)
    return level_ + 1;
}

void PlayerTank::updateShield() {
    if (hasShield_) {
        shieldTimer_--;
        if (shieldTimer_ <= 0) {
            deactivateShield();
        }
    }
}

void PlayerTank::renderShield(Renderer& renderer) const {
    int x = position_.pixelX();
    int y = position_.pixelY();

    // Shield blinking effect (2 frames alternating)
    int blinkFrame = (shieldTimer_ / 3) % 2; // 3 frames per blink phase
    uint8_t color = blinkFrame == 0 ? BattleCityPalette::COLOR_WHITE : BattleCityPalette::COLOR_CYAN;

    // Draw shield outline
    renderer.drawRect(x - 1, y - 1, 10, 10, color);
}

const uint8_t* PlayerTank::getSpriteData() const {
    // Return appropriate sprite based on direction and level
    // This would use TankSprites class
    // For now, return a placeholder
    static const uint8_t placeholder[64] = {
        0,0,0,1,1,0,0,0,
        0,0,1,1,1,1,0,0,
        0,1,1,1,1,1,1,0,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        0,1,1,1,1,1,1,0,
        0,0,1,1,1,1,0,0,
        0,0,0,1,1,0,0,0
    };
    return placeholder;
}

Vector2 PlayerTank::getSpawnPosition() const {
    // Return spawn position based on player index
    if (playerIndex_ == 0) {
        return Vector2::fromPixels(80, 200); // Player 1 spawn
    } else {
        return Vector2::fromPixels(160, 200); // Player 2 spawn
    }
}

BulletOwner PlayerTank::getBulletOwner() const {
    return (playerIndex_ == 0) ? BulletOwner::PLAYER_1 : BulletOwner::PLAYER_2;
}

} // namespace BattleCity