#pragma once

#include "Tank.h"
#include "../ai/AIController.h"

namespace BattleCity {

class EnemyTank : public Tank {
private:
    EnemyType type_;
    AIController aiController_;

public:
    EnemyTank(EnemyType type = EnemyType::BASIC, Game* game = nullptr);

    void update() override;
    void render(Renderer& renderer) override;
    void shoot() override;
    void destroy() override;

    EnemyType getType() const { return type_; }

    // AI helper methods
    bool wasHit() const { return false; } // TODO: Implement hit detection
    bool collidedWithPlayer() const { return false; } // TODO: Implement collision detection
    Vector2 getPlayerPosition() const; // TODO: Get from game state

protected:
    int getMoveSpeed() const override;
    int getShootCooldown() const override;
    int getBulletSpeed() const override;
    int getBulletPower() const override;
    BulletOwner getBulletOwner() const override;
};

} // namespace BattleCity