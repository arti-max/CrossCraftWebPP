#include "mob/ai/CreeperAI.hpp"
#include "mob/Mob.hpp"
#include "mob/Creeper.hpp"
#include "level/Level.hpp"
#include "CrossCraft.hpp"
#include "particle/TileParticle.hpp"

CreeperAI::CreeperAI(Creeper* creep) {
    this->creeper = creep;
}

bool CreeperAI::attack(Entity* e) {
    if (!BasicAttackAI::attack(e)) {
        return false;
    } else {
        this->creeper->hurt(e, 4);
        return true;
    }
}

void CreeperAI::beforeRemove() {
    float explosionRadius = 4.0f;

    this->level->explode(this->mob, this->mob->x, this->mob->y, this->mob->z, explosionRadius);

    for (int i = 0; i < 500; ++i) {
        float offsetX = (float)this->random.nextGaussian() * explosionRadius / 4.0f;
        float offsetY = (float)this->random.nextGaussian() * explosionRadius / 4.0f;
        float offsetZ = (float)this->random.nextGaussian() * explosionRadius / 4.0f;

        float distance = (float) std::sqrt((float)(offsetX*offsetX+offsetY*offsetY+offsetZ*offsetZ));

        float motionX = offsetX / distance / distance;
        float motionY = offsetY / distance / distance;
        float motionZ = offsetZ / distance / distance;

        this->level->cc->particleEngine->add(new TileParticle(this->level, this->mob->x + offsetX, this->mob->y + offsetY, this->mob->z + offsetZ, motionX, motionY, motionZ, (Tile*)Tile::leaves));
    }
}
