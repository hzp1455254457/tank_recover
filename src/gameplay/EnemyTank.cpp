#include "EnemyTank.h"

namespace BattleCity {

EnemyTank::EnemyTank(EnemyType type, Game* game) : Tank(game), type_(type) {
    aiController_.init(type_);
    // Initialize based on type
    switch (type_) {
        case EnemyType::BASIC:
            health_ = 1;
            break;
        case EnemyType::FAST:
            health_ = 1;
            break;
        case EnemyType::HEAVY:
            health_ = 2;
            break;
        case EnemyType::ELITE:
            health_ = 2;
            break;
    }

    // Set initial stats
    moveSpeed_ = getMoveSpeed();
    shootCooldown_ = getShootCooldown();
    bulletSpeed_ = getBulletSpeed();
    bulletPower_ = getBulletPower();
}

void EnemyTank::update() {
    updateInvincibility();
    updateCooldown();

    // Use AI controller for intelligent behavior
    aiController_.update(*this);

    updateAnimation();
}

void EnemyTank::render(Renderer& renderer) {
    if (!isActive_) return;

    int x = position_.pixelX();
    int y = position_.pixelY();

    // Render tank sprite based on type
    // TODO: Use proper sprite data
    renderer.fillRect(x, y, 8, 8, BattleCityPalette::COLOR_GRAY);
}

void EnemyTank::shoot() {
    if (!canShoot_ || currentCooldown_ > 0 || !isActive_) return;

    // Create bullet
    // TODO: Implement bullet creation
    currentCooldown_ = shootCooldown_;
}

void EnemyTank::destroy() {
    isActive_ = false;
    // TODO: Add destruction animation and sound effects
}

int EnemyTank::getMoveSpeed() const {
    switch (type_) {
        case EnemyType::BASIC: return 256;     // 1 pixel/frame
        case EnemyType::FAST: return 384;      // 1.5 pixels/frame
        case EnemyType::HEAVY: return 179;     // 0.7 pixels/frame
        case EnemyType::ELITE: return 307;     // 1.2 pixels/frame
        default: return 256;
    }
}

int EnemyTank::getShootCooldown() const {
    switch (type_) {
        case EnemyType::BASIC: return 120;     // 2 seconds
        case EnemyType::FAST: return 90;       // 1.5 seconds
        case EnemyType::HEAVY: return 110;     // ~1.8 seconds
        case EnemyType::ELITE: return 60;      // 1 second
        default: return 120;
    }
}

int EnemyTank::getBulletSpeed() const {
    return 3; // 3 pixels/frame
}

int EnemyTank::getBulletPower() const {
    return 1; // Basic penetration
}

BulletOwner EnemyTank::getBulletOwner() const {
    return BulletOwner::ENEMY;
}

Vector2 EnemyTank::getPlayerPosition() const {
    // TODO: Get actual player position from game
    // For now, return a default position
    return Vector2::fromPixels(120, 200); // Approximate player position
}

} // namespace BattleCity