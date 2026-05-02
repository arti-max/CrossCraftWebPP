#pragma once
#include "mob/HumanMob.hpp"
#include "mob/ai/BasicAttackAI.hpp"

class Level;

class Zombie : public HumanMob {
public:
    Zombie(Level* level, float x, float y, float z) : HumanMob(level, x, y, z) {
        this->modelName = "zombie";
        this->textureName = "/mob/zombie.png";
        this->heightOffset = 1.62f;
        BasicAttackAI* attackAi = new BasicAttackAI();
        attackAi->defaultLookAngle = 30;
        attackAi->runSpeed = 1.0f;
        this->ai = attackAi;
    }

    void die(Entity* e) {
        if (e != nullptr) {
            // TODO: add score
        }
        HumanMob::die(e);
    }
};