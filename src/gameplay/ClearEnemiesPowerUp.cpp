#include "ClearEnemiesPowerUp.h"
#include "../core/Game.h"

namespace BattleCity {

void ClearEnemiesPowerUp::activate(Game& game) {
    // 清除当前屏幕内的所有敌方坦克
    // 这里需要访问敌方坦克列表，暂时简化实现
    // TODO: 在Game类中添加clearAllEnemies方法
    // game.clearAllEnemies();

    // 临时实现：直接在Game中处理
    deactivate();
}

const uint8_t* ClearEnemiesPowerUp::getSpriteData() const {
    // 炸弹道具精灵
    static const uint8_t bombSprite[64] = {
        0,0,1,1,1,1,0,0,
        0,1,0,0,0,0,1,0,
        1,0,0,1,1,0,0,1,
        1,0,1,1,1,1,0,1,
        0,1,1,0,0,1,1,0,
        1,0,0,1,1,0,0,1,
        1,0,1,1,1,1,0,1,
        0,1,1,0,0,1,1,0
    };

    return bombSprite + ((animationFrame_ % 2) * 8);
}

} // namespace BattleCity