#pragma once
#include "mob/ai/BasicAI.hpp"

class BasicAttackAI : public BasicAI {
public:
    int damage = 6;

    void update() override;
    void doAttack();
    bool attack(Entity* e);
    void hurt(Entity* e, int dmg) override;
};