#pragma once

#include "PowerUp.h"

namespace BattleCity {

// 坦克升级道具（星星）
class TankUpgradePowerUp : public PowerUp {
public:
    TankUpgradePowerUp() : PowerUp(PowerUpType::TANK_UPGRADE) {}

    void activate(Game& game) override;

private:
    const uint8_t* getSpriteData() const override;
};

} // namespace BattleCity