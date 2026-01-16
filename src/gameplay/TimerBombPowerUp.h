#pragma once

#include "PowerUp.h"

namespace BattleCity {

// 定时炸弹道具（时钟）
class TimerBombPowerUp : public PowerUp {
public:
    TimerBombPowerUp() : PowerUp(PowerUpType::TIMER_BOMB) {}

    void activate(Game& game) override;

private:
    const uint8_t* getSpriteData() const override;
};

} // namespace BattleCity