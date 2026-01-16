#include "AIController.h"
#include "../gameplay/EnemyTank.h"
#include <cstdlib>

namespace BattleCity {

AIController::AIController()
    : currentState_(AIState::IDLE), stateTimer_(0), chaseTimer_(0),
      moveDirection_(Direction::UP), directionChangeTimer_(0) {
}

void AIController::init(EnemyType type) {
    switch (type) {
        case EnemyType::BASIC:
            sightRange_ = 64;
            directionChangeInterval_ = 250; // 150-300 frames
            chaseTimeout_ = 300;
            evadeDuration_ = 20;
            break;
        case EnemyType::FAST:
            sightRange_ = 64;
            directionChangeInterval_ = 200; // 100-250 frames
            chaseTimeout_ = 250;
            evadeDuration_ = 20;
            break;
        case EnemyType::HEAVY:
            sightRange_ = 64;
            directionChangeInterval_ = 400; // 250-500 frames
            chaseTimeout_ = 350;
            evadeDuration_ = 20;
            break;
        case EnemyType::ELITE:
            sightRange_ = 96;
            directionChangeInterval_ = 250; // 180-300 frames
            chaseTimeout_ = 400;
            evadeDuration_ = 25;
            break;
    }
}

void AIController::update(EnemyTank& tank) {
    stateTimer_++;

    // Handle state-specific logic
    switch (currentState_) {
        case AIState::IDLE:
            updateIdle(tank);
            break;
        case AIState::CHASE:
            updateChase(tank);
            break;
        case AIState::EVADE:
            updateEvade(tank);
            break;
        case AIState::FROZEN:
            updateFrozen(tank);
            break;
    }

    // Check for state transitions (priority: EVADE > CHASE > IDLE)
    // TODO: Implement hit and collision detection
    // if (tank.wasHit() || tank.collidedWithPlayer()) {
    //     changeState(AIState::EVADE);
    // } else
    if (isPlayerInSight(tank)) {
        changeState(AIState::CHASE);
    } else if (currentState_ == AIState::CHASE && chaseTimer_ <= 0) {
        changeState(AIState::IDLE);
    }
}

void AIController::changeState(AIState newState) {
    if (currentState_ == newState) return;

    currentState_ = newState;
    stateTimer_ = 0;

    if (newState == AIState::CHASE) {
        chaseTimer_ = chaseTimeout_;
    }
}

bool AIController::isPlayerInSight(const EnemyTank& tank) const {
    // Simplified: check distance to player
    Vector2 playerPos = tank.getPlayerPosition(); // Get from tank
    Vector2 delta = playerPos - tank.getPosition();

    int distance = MathUtils::distance(0, 0, delta.pixelX(), delta.pixelY());
    return distance <= sightRange_;
}

void AIController::updateIdle(EnemyTank& tank) {
    // Random movement with occasional direction changes
    directionChangeTimer_--;
    if (directionChangeTimer_ <= 0) {
        // Choose random direction
        Direction dirs[4] = {Direction::UP, Direction::DOWN, Direction::LEFT, Direction::RIGHT};
        moveDirection_ = dirs[rand() % 4];
        directionChangeTimer_ = directionChangeInterval_ + (rand() % 100 - 50); // Add variance
    }

    tank.setDirection(moveDirection_);
    tank.move();

    // Random shooting (30% chance for basic, varies by type)
    int shootChance = 30; // Basic default
    if (rand() % 100 < shootChance) {
        tank.shoot();
    }
}

void AIController::updateChase(EnemyTank& tank) {
    // Move towards player
    Direction chaseDir = Direction::RIGHT; // TODO: Implement proper chase direction
    tank.setDirection(chaseDir);
    tank.move();

    // Shoot at player if aligned (simplified)
    if ((rand() % 100) < 30) { // 30% chance to shoot when chasing
        tank.shoot();
    }

    chaseTimer_--;
}

void AIController::updateEvade(EnemyTank& tank) {
    // Move away from player for evade duration
    Direction evadeDir = getOppositeDirection(tank.getDirection()); // Evade from current direction
    tank.setDirection(evadeDir);
    tank.move();

    if (stateTimer_ >= evadeDuration_) {
        changeState(AIState::IDLE);
    }
}

void AIController::updateFrozen(EnemyTank& tank) {
    // Do nothing - tank is frozen by timer bomb
}

Direction AIController::calculateChaseDirection(const EnemyTank& tank) const {
    Vector2 playerPos = Vector2::fromPixels(120, 200); // Placeholder player position
    Vector2 tankPos = tank.getPosition();
    Vector2 delta = playerPos - tankPos;

    // Prioritize horizontal alignment, then vertical
    if (abs(delta.pixelX()) > abs(delta.pixelY())) {
        return delta.pixelX() > 0 ? Direction::RIGHT : Direction::LEFT;
    } else {
        return delta.pixelY() > 0 ? Direction::DOWN : Direction::UP;
    }
}

Direction AIController::getOppositeDirection(Direction dir) const {
    switch (dir) {
        case Direction::UP: return Direction::DOWN;
        case Direction::DOWN: return Direction::UP;
        case Direction::LEFT: return Direction::RIGHT;
        case Direction::RIGHT: return Direction::LEFT;
        default: return Direction::UP;
    }
}

bool AIController::hasLineOfSight(const EnemyTank& tank, const Vector2& target) const {
    // Simplified line of sight check
    // In full implementation, would check terrain obstacles
    return true;
}

// Helper functions (would be implemented with game state access)

bool AIController::isAlignedWithPlayer(const EnemyTank& tank) const {
    // Simplified alignment check
    Vector2 playerPos = Vector2::fromPixels(120, 200); // Placeholder player position
    Vector2 tankPos = tank.getPosition();
    Direction tankDir = tank.getDirection();

    // Check if player is in line of fire
    switch (tankDir) {
        case Direction::UP:
            return tankPos.pixelX() == playerPos.pixelX() && playerPos.pixelY() < tankPos.pixelY();
        case Direction::DOWN:
            return tankPos.pixelX() == playerPos.pixelX() && playerPos.pixelY() > tankPos.pixelY();
        case Direction::LEFT:
            return tankPos.pixelY() == playerPos.pixelY() && playerPos.pixelX() < tankPos.pixelX();
        case Direction::RIGHT:
            return tankPos.pixelY() == playerPos.pixelY() && playerPos.pixelX() > tankPos.pixelX();
        default:
            return false;
    }
}

} // namespace BattleCity