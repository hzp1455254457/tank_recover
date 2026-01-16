#include "PowerUp.h"
#include "Tank.h"
#include "TankUpgradePowerUp.h"
#include "LifeBonusPowerUp.h"
#include "ShieldPowerUp.h"
#include "TimerBombPowerUp.h"
#include "ClearEnemiesPowerUp.h"
#include "../graphics/Renderer.h"
#include "../core/Game.h"
#include <memory>
#include <cstdlib>

namespace BattleCity {

PowerUp::PowerUp(PowerUpType type)
    : type_(type), isActive_(true), lifetime_(600), // 10秒存在时间
      animationFrame_(0), animationTimer_(0) {
}

void PowerUp::update() {
    if (!isActive_) return;

    lifetime_--;
    if (lifetime_ <= 0) {
        deactivate();
        return;
    }

    updateAnimation();
}

void PowerUp::render(Renderer& renderer) const {
    if (!isActive_) return;

    int x = position_.pixelX() - 4; // 8x8精灵居中
    int y = position_.pixelY() - 4;

    const uint8_t* spriteData = getSpriteData();
    uint8_t colorIndex = 0x20; // 默认白色

    // 根据道具类型设置颜色
    switch (type_) {
        case PowerUpType::TANK_UPGRADE: colorIndex = 0x28; break; // 金色
        case PowerUpType::EXTRA_LIFE:   colorIndex = 0x25; break; // 绿色
        case PowerUpType::TIMER_BOMB:   colorIndex = 0x24; break; // 粉色
        case PowerUpType::SHIELD:       colorIndex = 0x2B; break; // 亮绿
        case PowerUpType::CLEAR_ENEMIES:colorIndex = 0x06; break; // 橙色
    }

    renderer.drawSprite(x, y, spriteData, colorIndex);
}

Rect PowerUp::getBounds() const {
    return Rect(position_.pixelX() - 4, position_.pixelY() - 4, 8, 8);
}

bool PowerUp::collidesWithTank(const Tank& tank) const {
    if (!isActive_) return false;

    Rect powerUpBounds = getBounds();
    Rect tankBounds = tank.getBounds();

    return MathUtils::rectsIntersect(powerUpBounds, tankBounds);
}

std::unique_ptr<PowerUp> PowerUp::createRandomPowerUp() {
    // 道具生成概率（参考原版）
    // 坦克升级: 25%, 生命加成: 25%, 护盾: 20%, 定时炸弹: 15%, 清屏炸弹: 15%
    int rand = std::rand() % 100;

    PowerUpType type;
    if (rand < 25) {
        type = PowerUpType::TANK_UPGRADE;
    } else if (rand < 50) {
        type = PowerUpType::EXTRA_LIFE;
    } else if (rand < 70) {
        type = PowerUpType::SHIELD;
    } else if (rand < 85) {
        type = PowerUpType::TIMER_BOMB;
    } else {
        type = PowerUpType::CLEAR_ENEMIES;
    }

    // 创建具体道具类型
    switch (type) {
        case PowerUpType::TANK_UPGRADE:
            return std::make_unique<TankUpgradePowerUp>();
        case PowerUpType::EXTRA_LIFE:
            return std::make_unique<LifeBonusPowerUp>();
        case PowerUpType::TIMER_BOMB:
            return std::make_unique<TimerBombPowerUp>();
        case PowerUpType::SHIELD:
            return std::make_unique<ShieldPowerUp>();
        case PowerUpType::CLEAR_ENEMIES:
            return std::make_unique<ClearEnemiesPowerUp>();
        default:
            return nullptr;
    }
}

void PowerUp::updateAnimation() {
    animationTimer_++;
    if (animationTimer_ >= 10) { // 10帧切换一次
        animationFrame_ = (animationFrame_ + 1) % 4; // 4帧动画
        animationTimer_ = 0;
    }
}

const uint8_t* PowerUp::getSpriteData() const {
    // 默认道具精灵（星星形状）
    static const uint8_t starSprite[64] = {
        // 帧0
        0,1,0,0,0,0,1,0,
        1,1,1,0,0,1,1,1,
        0,1,0,0,0,0,1,0,
        0,0,0,0,0,0,0,0,
        // 帧1
        0,0,1,0,0,1,0,0,
        0,1,1,1,1,1,1,0,
        1,0,1,0,0,1,0,1,
        0,0,0,0,0,0,0,0
    };

    return starSprite + (animationFrame_ * 8);
}

} // namespace BattleCity