#pragma once
#include "mob/ai/BasicAttackAI.hpp"

class Creeper;

class CreeperAI : public BasicAttackAI {
public:
    Creeper* creeper;

    CreeperAI(Creeper* creep);

    bool attack(Entity* e) override;
    void beforeRemove() override;
};