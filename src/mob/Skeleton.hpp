#pragma once
#include "mob/Zombie.hpp"
#include "item/Arrow.hpp"
#include "util/Random.hpp"
#include "mob/ai/SkeletonAI.hpp"

class Level;

class Skeleton : public Zombie {
public:
    Skeleton(Level* level, float x, float y, float z) : Zombie(level, x, y, z) {
        this->modelName = "skeleton";
        this->textureName = "/mob/skeleton.png";
        if (this->ai != nullptr) {
            delete this->ai; 
        }
        SkeletonAI* ai = new SkeletonAI(this);
        ai->runSpeed = 0.3f;
        this->ai = ai;
    }

    EntityType getEntityType() const override { return EntityType::Skeleton; }
    void shootArrow() {
        this->level->addEntity(new Arrow(this->level, (Entity*)this, this->x, this->y, this->z, this->yRot + 180.0f + (Random::random() * 45.0f - 22.5f), this->xRot - (Random::random() * 45.0f - 10.0f), 1.0f));
    }

    static void shootRandomArrow(Skeleton* skeleton) {
        int cnt = ((Random::random() + Random::random()) * 3.0f + 4.0f);

        for (int i = 0; i < cnt; i++) {
            skeleton->level->addEntity(new Arrow(skeleton->level, skeleton->level->getPlayer(), skeleton->x, skeleton->y - 0.2f, skeleton->z, Random::random()*360.0f, -(Random::random()) * 60.0f, 0.4f));
        }
    }
};