#include "ShieldPowerUp.h"
#include "../core/Game.h"
#include "../gameplay/PlayerTank.h"

namespace BattleCity {

void ShieldPowerUp::activate(Game& game) {
    // 为玩家激活护盾保护
    auto* player1 = game.getPlayer1();
    if (player1 && player1->isActive()) {
        player1->activateShield(180); // 3秒护盾 (180帧)
    }

    deactivate();
}

const uint8_t* ShieldPowerUp::getSpriteData() const {
    // 护盾道具精灵
    static const uint8_t shieldSprite[64] = {
        // 帧0
        0,0,1,1,1,1,0,0,
        0,1,0,0,0,0,1,0,
        1,0,0,0,0,0,0,1,
        1,0,0,0,0,0,0,1,
        // 帧1
        0,1,1,0,0,1,1,0,
        1,0,0,0,0,0,0,1,
        1,0,0,0,0,0,0,1,
        0,1,1,0,0,1,1,0
    };

    return shieldSprite + ((animationFrame_ % 2) * 8);
}

} // namespace BattleCity