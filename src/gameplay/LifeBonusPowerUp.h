#pragma once

#include "PowerUp.h"

namespace BattleCity {

// 生命加成道具（坦克图标）
class LifeBonusPowerUp : public PowerUp {
public:
    LifeBonusPowerUp() : PowerUp(PowerUpType::EXTRA_LIFE) {}

    void activate(Game& game) override;

private:
    const uint8_t* getSpriteData() const override;
};

} // namespace BattleCity