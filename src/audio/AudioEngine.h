#pragma once

#include <cstdint>

namespace BattleCity {

enum class SoundEffect {
    TANK_MOVE,
    TANK_SHOOT,
    BULLET_HIT,
    BRICK_DESTROY,
    BASE_DESTROY,
    TANK_DESTROY,
    POWERUP_PICKUP,
    UPGRADE,
    TIMER_BOMB,
    SHIELD,
    CLEAR_ENEMIES,
    EXTRA_LIFE
};

class AudioEngine {
public:
    static void init() {}
    static void playSound(SoundEffect effect) {}
    static void playMusic(bool loop = true) {}
    static void pauseMusic() {}
    static void resumeMusic() {}
    static void pauseAll() {}
    static void resumeAll() {}
};

} // namespace BattleCity