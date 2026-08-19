#pragma once
#include "mob/Mob.hpp"
#include "mob/ai/CreeperAI.hpp"

class Creeper : public Mob {
public:
    Creeper(Level* level, float x, float y, float z) : Mob(level) {
        this->heightOffset = 1.62f;
        this->modelName = "creeper";
        this->textureName = "/mob/creeper.png";
        if (this->ai != nullptr) {
            delete this->ai; 
        }
        this->ai = new CreeperAI(this);
        this->ai->defaultLookAngle = 45;
        this->setPos(x, y, z);
    }

    float getBrightness(float partialTicks) override {
        float healthFactor = (float)(20 - this->health) / 20.0f;
        return ((std::sin((float)(this->tickCount + partialTicks)) * 0.5f + 0.5f) * healthFactor * 0.5f + 0.25f + healthFactor * 0.25f) * Mob::getBrightness(partialTicks);
    }

    void die(Entity* e) override {
        if (e != nullptr) {
            e->awardKillScore(this, 250);
        }

        Mob::die(e);
    }
};