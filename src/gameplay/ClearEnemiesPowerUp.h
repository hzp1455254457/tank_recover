#pragma once

#include "PowerUp.h"

namespace BattleCity {

// 全屏清敌道具（炸弹）
class ClearEnemiesPowerUp : public PowerUp {
public:
    ClearEnemiesPowerUp() : PowerUp(PowerUpType::CLEAR_ENEMIES) {}

    void activate(Game& game) override;

private:
    const uint8_t* getSpriteData() const override;
};

} // namespace BattleCity