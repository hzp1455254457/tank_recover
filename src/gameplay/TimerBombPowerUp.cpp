#include "TimerBombPowerUp.h"
#include "../core/Game.h"
#include "../gameplay/EnemyTank.h"

namespace BattleCity {

void TimerBombPowerUp::activate(Game& game) {
    // 让所有敌方坦克静止10秒
    // 这里需要访问敌方坦克列表，暂时简化实现
    // TODO: 在Game类中添加freezeEnemies方法
    // game.freezeEnemies(600); // 10秒 = 600帧

    // 临时实现：直接在Game中处理
    deactivate();
}

const uint8_t* TimerBombPowerUp::getSpriteData() const {
    // 时钟道具精灵
    static const uint8_t clockSprite[64] = {
        // 帧0
        0,1,1,1,1,1,1,0,
        1,0,0,0,0,0,0,1,
        1,0,1,1,1,1,0,1,
        1,0,1,1,1,1,0,1,
        // 帧1
        0,1,1,1,1,1,1,0,
        1,0,0,0,0,0,0,1,
        1,0,0,1,1,0,0,1,
        1,0,0,1,1,0,0,1
    };

    return clockSprite + ((animationFrame_ % 2) * 8);
}

} // namespace BattleCity