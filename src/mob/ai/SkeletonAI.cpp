#include "mob/ai/SkeletonAI.hpp"
#include "mob/Mob.hpp"
#include "level/Level.hpp"
#include "mob/Skeleton.hpp"

SkeletonAI::SkeletonAI(Skeleton* skeleton) {
    this->parent = skeleton;
}

void SkeletonAI::tick(Level* level, Mob* mob) {
    BasicAttackAI::tick(level, mob);

    if (mob->health > 0 && this->random->nextInt(30) == 0 && this->attackTarget != nullptr) {
        this->parent->shootArrow();
    }
}

void SkeletonAI::beforeRemove() {
    Skeleton::shootRandomArrow(this->parent);
}