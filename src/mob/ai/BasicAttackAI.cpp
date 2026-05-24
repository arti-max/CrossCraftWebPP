#include "mob/ai/BasicAttackAI.hpp"
#include "mob/Mob.hpp"
#include "level/Level.hpp"
#include "model/Vec3D.hpp"

void BasicAttackAI::update() {
    BasicAI::update();
    if (this->mob->health > 0) {
        this->doAttack();
    }
}

void BasicAttackAI::doAttack() {
    Entity* player = this->level->getPlayer();
    float searchRange = 16.0f;

    if (this->attackTarget != nullptr && this->attackTarget->removed) {
        this->attackTarget = nullptr;
    }

    float dx;
    float dy;
    float dz;

    if (player != nullptr && this->attackTarget == nullptr) {
        dx = player->x - this->mob->x;
        dy = player->y - this->mob->y;
        dz = player->z - this->mob->z;
        if (dx*dx + dy*dy + dz*dz < searchRange*searchRange) {
            this->attackTarget = player;
        }
    }
            
    if (this->attackTarget != nullptr) {
        dx = this->attackTarget->x - this->mob->x;
        dy = this->attackTarget->y - this->mob->y;
        dz = this->attackTarget->z - this->mob->z;
        float distanceSqr = dx*dx + dy*dy + dz*dz;
        if (distanceSqr > searchRange*searchRange*2.0f*2.0f && this->random->nextInt(100) == 0) {
            this->attackTarget = nullptr;
        }

        if (this->attackTarget != nullptr) {
            float distance = std::sqrt(distanceSqr);
            this->mob->yRot = (float)(std::atan2(dz, dx) * 180.0f / M_PI) - 90.0f;
            this->mob->xRot = -((float)(std::atan2(dy, distance) * 180.0f / M_PI));
            if (distance < 2.0f && this->attackDelay == 0) {
                this->attack(this->attackTarget);
            }
        }
    }

}

bool BasicAttackAI::attack(Entity* target) {
    Vec3D mobVec = Vec3D(this->mob->x, this->mob->y, this->mob->z);
    Vec3D targetVec = Vec3D(target->x, target->y, target->z);
    if (this->level->clip(mobVec, targetVec) != nullptr) {
        return false;
    } else {
        this->mob->attackTime = 5;
        this->attackDelay = this->random->nextInt(20) + 10;
        // int dmg = (this->random->nextFloat() + this->random->nextFloat()) / 2.0f * (float)this->damage + 1.0f;
        int dmg = this->random->nextInt(4) + this->random->nextInt(4) + 1;
        target->hurt(this->mob, dmg);
        this->noActionTime = 0;
        return true;
    }
}

void BasicAttackAI::hurt(Entity* target, int dmg) {
    BasicAI::hurt(target, dmg);

    if (target != nullptr && !(target->getEntityType() == this->mob->getEntityType())) {
        this->attackTarget = target;
    }
}