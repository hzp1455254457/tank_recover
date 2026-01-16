#pragma once

#include "../utils/MathUtils.h"
#include <memory>

namespace BattleCity {

class Game; // Forward declaration
class Renderer;

enum class PowerUpType {
    TANK_UPGRADE,    // 坦克升级（星星）
    EXTRA_LIFE,      // 生命加成（坦克图标）
    TIMER_BOMB,      // 定时炸弹（时钟）
    SHIELD,          // 护盾保护（护盾）
    CLEAR_ENEMIES    // 全屏清敌（炸弹）
};

// 道具基类
class PowerUp {
protected:
    Vector2 position_;           // 道具位置
    PowerUpType type_;          // 道具类型
    bool isActive_;             // 是否激活
    int lifetime_;              // 存在时间（帧）
    int animationFrame_;        // 动画帧
    int animationTimer_;        // 动画计时器

public:
    PowerUp(PowerUpType type);
    virtual ~PowerUp() = default;

    // 核心方法
    virtual void update();
    virtual void render(class Renderer& renderer) const;
    virtual void activate(class Game& game) = 0; // 激活道具效果

    // 获取器
    Vector2 getPosition() const { return position_; }
    PowerUpType getType() const { return type_; }
    bool isActive() const { return isActive_; }
    Rect getBounds() const;

    // 设置器
    void setPosition(const Vector2& pos) { position_ = pos; }
    void deactivate() { isActive_ = false; }

    // 碰撞检测
    bool collidesWithTank(const class Tank& tank) const;

    // 静态方法：创建随机道具
    static std::unique_ptr<PowerUp> createRandomPowerUp();

private:
    // 动画更新
    void updateAnimation();

    // 获取精灵数据
    virtual const uint8_t* getSpriteData() const;
};

} // namespace BattleCity