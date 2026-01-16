#pragma once

#include "../utils/MathUtils.h"
#include <functional>

namespace BattleCity {

enum class AIState {
    IDLE,       // 漫游状态
    CHASE,      // 追击状态
    EVADE,      // 躲避状态
    FROZEN      // 定时炸弹静止状态
};

class AIController {
private:
    AIState currentState_;
    int stateTimer_;
    int chaseTimer_;
    Direction moveDirection_;
    int directionChangeTimer_;
    Vector2 lastPlayerPosition_;

    // AI parameters (vary by enemy type)
    int sightRange_;
    int directionChangeInterval_;
    int chaseTimeout_;
    int evadeDuration_;

public:
    AIController();
    void init(EnemyType type);

    // Update AI logic
    void update(class EnemyTank& tank);

    // State transitions
    void changeState(AIState newState);

    // Getters
    AIState getCurrentState() const { return currentState_; }
    bool isPlayerInSight(const class EnemyTank& tank) const;

private:
    void updateIdle(class EnemyTank& tank);
    void updateChase(class EnemyTank& tank);
    void updateEvade(class EnemyTank& tank);
    void updateFrozen(class EnemyTank& tank);

    Direction calculateChaseDirection(const class EnemyTank& tank) const;
    Direction getOppositeDirection(Direction dir) const;
    bool hasLineOfSight(const class EnemyTank& tank, const Vector2& target) const;
    bool isAlignedWithPlayer(const class EnemyTank& tank) const;
};

} // namespace BattleCity