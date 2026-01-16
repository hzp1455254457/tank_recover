#pragma once

#include "PowerUp.h"

namespace BattleCity {

// 护盾保护道具（护盾）
class ShieldPowerUp : public PowerUp {
public:
    ShieldPowerUp() : PowerUp(PowerUpType::SHIELD) {}

    void activate(Game& game) override;

private:
    const uint8_t* getSpriteData() const override;
};

} // namespace BattleCity