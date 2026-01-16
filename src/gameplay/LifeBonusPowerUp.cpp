#include "LifeBonusPowerUp.h"
#include "../core/Game.h"
#include "../gameplay/PlayerTank.h"

namespace BattleCity {

void LifeBonusPowerUp::activate(Game& game) {
    // 为拾取道具的玩家增加生命
    // 简化实现：为玩家1增加生命
    auto* player1 = game.getPlayer1();
    if (player1 && player1->isActive()) {
        player1->gainLife();
        // TODO: 添加生命增加音效和视觉效果
    }

    deactivate();
}

const uint8_t* LifeBonusPowerUp::getSpriteData() const {
    // 坦克图标道具精灵
    static const uint8_t tankSprite[64] = {
        // 帧0
        0,1,1,1,1,1,1,0,
        1,1,0,0,0,0,1,1,
        1,0,0,0,0,0,0,1,
        1,0,0,0,0,0,0,1,
        // 帧1
        0,1,1,1,1,1,1,0,
        1,1,0,0,0,0,1,1,
        1,0,0,0,0,0,0,1,
        1,0,0,0,0,0,0,1
    };

    return tankSprite + ((animationFrame_ % 2) * 8);
}

} // namespace BattleCity