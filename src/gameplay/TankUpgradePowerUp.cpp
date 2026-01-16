#include "TankUpgradePowerUp.h"
#include "../core/Game.h"
#include "../gameplay/PlayerTank.h"

namespace BattleCity {

void TankUpgradePowerUp::activate(Game& game) {
    // 为所有活跃玩家坦克升级
    // 简化实现：只为玩家1升级（可扩展为双人）
    auto* player1 = game.getPlayer1();
    if (player1 && player1->isActive()) {
        player1->upgrade();
        // TODO: 添加升级音效和视觉效果
    }

    deactivate();
}

const uint8_t* TankUpgradePowerUp::getSpriteData() const {
    // 星星道具精灵
    static const uint8_t starSprite[64] = {
        // 帧0
        0,0,1,0,0,1,0,0,
        0,1,0,0,0,0,1,0,
        1,0,0,1,1,0,0,1,
        0,0,1,0,0,1,0,0,
        // 帧1
        0,1,0,0,0,0,1,0,
        1,0,0,1,1,0,0,1,
        0,0,1,0,0,1,0,0,
        0,0,0,0,0,0,0,0
    };

    return starSprite + ((animationFrame_ % 2) * 8);
}

} // namespace BattleCity