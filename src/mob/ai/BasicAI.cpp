#include "mob/ai/BasicAI.hpp"
#include "level/Level.hpp"
#include "Entity.hpp"
#include "mob/Mob.hpp"

void BasicAI::tick(Level* level, Mob* mob) {
    ++this->noActionTime;
    Entity* player = level->getPlayer();
    if (this->noActionTime > 600 && this->random->nextInt(800) == 0 && (player != nullptr)) {
        float dx = player->x - mob->x;
    }
}