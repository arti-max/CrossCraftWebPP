#pragma once
#include "CrossCraft.hpp"
#include "Entity.hpp"
#include "level/Level.hpp"
#include "liquid/LiquidType.hpp"
#include "particle/SmokeParticle.hpp"
#include "util/Random.hpp"

class Smolder : Entity {
private:
    int lifeTime = 0;
    int life = 0;
public:
    Smolder(Level * level, int x, int y, int z) : Entity(level) {
        this->setSize(0.0f, 0.0f);
        this->setPos((float)x + 0.5f, (float)y + 0.1f, (float) z + 0.5f);
        this->heightOffset = 1.5f;
        this->makeStepSound = false;
        this->lifeTime = (int)(40.0f / (Random::random() * 0.8f + 0.2f));
    }

    bool isPickable() override {
        return !this->removed;
    }

    void tick() override {
        float offsetX = (float)(Random::random() - 0.5f);
        float offsetZ = (float)(Random::random() - 0.5f);
        float chance = (float)this->life / (float)this->lifeTime;

        for (int i = 0; i < 4; ++i) {
            if (Random::random() > chance) {
                CrossCraft::instance->particleEngine->add(new SmokeParticle(this->level, this->x + offsetX, this->y, this->z + offsetZ));
            }
        }

        int tileHere = this->level->getTile((int)this->x, (int)this->y, (int)this->z);
        if (tileHere > 0 && Tile::tiles[tileHere]->getLiquidType() != LiquidType::NOT_LIQUID) {
            this->remove();
        }

        if (this->life++ < this->lifeTime) {
            int x = (int)this->x;
            int y = (int)this->y - 0.3f;
            int z = (int)this->z;

            int tileId = this->level->getTile(x, y, z);
            if (tileId > 0 && Tile::tiles[tileId]->isSolid()) {
                return;
            }
        }

        this->remove();
    }
};