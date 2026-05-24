#pragma once
#include "mob/Mob.hpp"

class AnimalMob : public Mob {
public:
    AnimalMob(Level* level, float x, float y, float z) : Mob(level) {
        this->setSize(1.4f, 1.4f);
        this->setPos(x, y, z);
        this->modelName = "pig";
        this->heightOffset = 1.72f;
        this->textureName = "/mob/pig.png";
    }

    void die(Entity* e) override {
        if (e != nullptr) {
            e->awardKillScore(this, 10);
        }

        Mob::die(e);
    }
};