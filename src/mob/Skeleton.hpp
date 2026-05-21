#pragma once
#include "mob/Zombie.hpp"

class Level;

class Skeleton : public Zombie {
public:
    Skeleton(Level* level, float x, float y, float z) : Zombie(level, x, y, z) {
        this->modelName = "skeleton";
        this->textureName = "/mob/skeleton.png";
        BasicAttackAI* ai = new BasicAttackAI();
        ai->runSpeed = 0.7f;
        this->ai = ai;
    }

    EntityType getEntityType() const override { return EntityType::Skeleton; }
};