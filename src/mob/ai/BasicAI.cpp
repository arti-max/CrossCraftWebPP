#include "mob/ai/BasicAI.hpp"
#include "level/Level.hpp"
#include "Entity.hpp"
#include "mob/Mob.hpp"

void BasicAI::tick(Level* level, Mob* mob) {
    ++this->noActionTime;
    Entity* player = level->getPlayer();
    if (this->noActionTime > 600 && this->random->nextInt(800) == 0 && (player != nullptr)) {
        float dx = player->x - mob->x;
        float dy = player->y - mob->y;
        float dz = player->z - mob->z;
        if (dx*dx + dy*dy + dz*dz < 1024.0f) {
            this->noActionTime = 0;
        } else {
            mob->remove();
        }
    }

    this->level = level;
    this->mob = mob;
    if (this->attackDelay > 0) {
        --this->attackDelay;
    }

    if (mob->health <= 0) {
        this->jumping = false;
        this->xxa = 0.0f;
        this->yya = 0.0f;
        this->yRotA = 0.0f;
    } else {
        this->update();
    }


    bool inWater = mob->isInWater();
    bool inLava = mob->isInLava();
    if (this->jumping) {
        if (inWater) {
            mob->yd += 0.04f;
        } else if (inLava) {
            mob->yd += 0.04f;
        } else if (mob->onGround) {
            this->jumpFromGround();
        }
    }

    this->xxa *= 0.98f;
    this->yya *= 0.98f;
    this->yRotA *= 0.9f;
    mob->travel(this->xxa, this->yya);
    std::vector<Entity*> entites = level->findEntities(mob, mob->bb.grow(0.2f, 0.0f, 0.2f));
    if (entites.size() > 0) {
        for (int i = 0; i < entites.size(); ++i) {
            Entity* e = entites[i];
            if (e->isPushable()) {
                e->push(mob);
            }

        }
    }
}

void BasicAI::jumpFromGround() {
    this->mob->yd = 0.42f;
}

void BasicAI::update() {
    if (this->random->nextFloat() < 0.07f) {
        this->xxa = (this->random->nextFloat() - 0.5f) * this->runSpeed;
        this->yya = this->random->nextFloat() * this->runSpeed;
    }

    this->jumping = this->random->nextFloat() < 0.01f;
    if (this->random->nextFloat() < 0.04f) {
        this->yRotA = (this->random->nextFloat() - 0.5f) * 60.0f;
    }

    this->mob->yRot += this->yRotA;
    this->mob->xRot = (float)this->defaultLookAngle;
    if (this->attackTarget != nullptr) {
        this->yya = this->runSpeed;
        this->jumping = this->random->nextFloat() < 0.04f;
    }

    bool inWater = this->mob->isInWater();
    bool inLava = this->mob->isInLava();
    if (inWater || inLava) {
        this->jumping = this->random->nextFloat() < 0.08f;
    }

}

void BasicAI::hurt(Entity* e, int dmg) {
    AI::hurt(e, dmg);
    this->noActionTime = 0;
}

void BasicAI::beforeRemove() {
    // nothing
}