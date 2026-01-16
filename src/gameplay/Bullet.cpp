#include "Bullet.h"
#include "Tank.h"
#include "../level/LevelManager.h"
#include "../graphics/Renderer.h"
#include <algorithm>

namespace BattleCity {

Bullet::Bullet()
    : direction_(Direction::UP), owner_(BulletOwner::PLAYER_1),
      speed_(384), power_(1), isActive_(false), lifetime_(0), maxLifetime_(180),
      animationFrame_(0), animationTimer_(0) {
}

void Bullet::init(const Vector2& position, Direction direction, BulletOwner owner, int power) {
    position_ = position;
    direction_ = direction;
    owner_ = owner;
    power_ = std::clamp(power, 1, 3);
    isActive_ = true;
    lifetime_ = maxLifetime_;
    animationFrame_ = 0;
    animationTimer_ = 0;

    // Set speed based on power (higher power bullets are slightly faster)
    speed_ = 384 + (power_ - 1) * 64; // Base 384, +64 per power level
}

void Bullet::update() {
    if (!isActive_) return;

    move();
    updateAnimation();

    lifetime_--;
    if (lifetime_ <= 0) {
        deactivate();
    }
}

void Bullet::move() {
    Vector2 velocity = MathUtils::directionToVelocity(direction_, speed_);
    position_ = position_ + velocity;
}

bool Bullet::checkCollisionWithTank(Tank& tank) {
    if (!isActive_ || !tank.isActive()) return false;

    // Skip collision with bullet owner
    if ((owner_ == BulletOwner::PLAYER_1 || owner_ == BulletOwner::PLAYER_2) &&
        tank.getDirection() != Direction::NONE) { // Assuming player tanks have direction != NONE
        // TODO: Add proper owner checking when Tank class has owner info
    }

    Rect bulletBounds = getBounds();
    Rect tankBounds = tank.getBounds();

    if (MathUtils::rectsIntersect(bulletBounds, tankBounds)) {
        tank.takeDamage(1);
        deactivate();
        return true;
    }

    return false;
}

bool Bullet::checkCollisionWithTerrain(LevelManager& levelManager) {
    if (!isActive_) return false;

    // Check the pixel position of the bullet
    int pixelX = position_.pixelX();
    int pixelY = position_.pixelY();

    // Check terrain collision
    TerrainType terrain = levelManager.getTerrain(pixelX, pixelY);
    if (terrain != TerrainType::GRASS && !canPenetrate(terrain)) {
        // Handle terrain destruction
        if (terrain == TerrainType::BRICK) {
            levelManager.destroyBrick(pixelX, pixelY);
        }
        deactivate();
        return true;
    }

    // Check screen boundaries
    if (pixelX < 0 || pixelX >= 256 || pixelY < 0 || pixelY >= 224) {
        deactivate();
        return true;
    }

    return false;
}

bool Bullet::checkCollisionWithBase(const Vector2& basePos) {
    if (!isActive_) return false;

    // Base collision box (16x16 pixels centered at basePos)
    Rect baseBounds = {
        basePos.pixelX() - 8, basePos.pixelY() - 8,
        16, 16
    };

    Rect bulletBounds = getBounds();

    if (MathUtils::rectsIntersect(bulletBounds, baseBounds)) {
        deactivate();
        return true; // Base destroyed
    }

    return false;
}

Rect Bullet::getBounds() const {
    // Bullet is 4x4 pixels
    return Rect{
        position_.pixelX() - 2,
        position_.pixelY() - 2,
        4, 4
    };
}

void Bullet::deactivate() {
    isActive_ = false;
}

void Bullet::setLifetime(int frames) {
    maxLifetime_ = frames;
    lifetime_ = frames;
}

void Bullet::render(Renderer& renderer) const {
    if (!isActive_) return;

    int x = position_.pixelX() - 2; // Center the 4x4 sprite
    int y = position_.pixelY() - 2;

    // Render bullet sprite based on direction and animation
    const uint8_t* spriteData = getSpriteData();
    uint8_t colorIndex = (owner_ == BulletOwner::ENEMY) ? 0x06 : 0x20; // Orange for enemy, white for player

    renderer.drawSprite(x, y, spriteData, colorIndex);
}

bool Bullet::isValidPosition(const Vector2& pos, LevelManager& levelManager) const {
    int pixelX = pos.pixelX();
    int pixelY = pos.pixelY();

    if (pixelX < 0 || pixelX >= 256 || pixelY < 0 || pixelY >= 224) {
        return false;
    }

    TerrainType terrain = levelManager.getTerrain(pixelX, pixelY);
    return terrain == TerrainType::GRASS || canPenetrate(terrain);
}

bool Bullet::canPenetrate(TerrainType terrain) const {
    switch (terrain) {
        case TerrainType::GRASS:
            return true;
        case TerrainType::BRICK:
            return false; // Bullets destroy bricks but are consumed
        case TerrainType::STEEL:
            return power_ >= 2; // Need level 2+ to penetrate steel
        case TerrainType::WATER:
            return true; // Bullets can travel through water
        default:
            return false;
    }
}

void Bullet::updateAnimation() {
    animationTimer_++;
    if (animationTimer_ >= 3) { // Change frame every 3 frames
        animationFrame_ = (animationFrame_ + 1) % 2;
        animationTimer_ = 0;
    }
}

const uint8_t* Bullet::getSpriteData() const {
    // Simple bullet sprites (4x4 pixels)
    static const uint8_t bulletSprites[2][16] = {
        // Frame 0
        {
            0,1,1,0,
            1,0,0,1,
            1,0,0,1,
            0,1,1,0
        },
        // Frame 1
        {
            1,0,0,1,
            0,1,1,0,
            0,1,1,0,
            1,0,0,1
        }
    };

    return bulletSprites[animationFrame_];
}

} // namespace BattleCity