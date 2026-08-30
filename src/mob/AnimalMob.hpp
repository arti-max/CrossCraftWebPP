#pragma once
#include "mob/Mob.hpp"
#include "item/Item.hpp"

#include "level/Level.hpp"
#include "level/tile/Tile.hpp"

class AnimalMob : public Mob {
public:
    AnimalMob(Level* level, float x, float y, float z) : Mob(level) {
        this->setSize(1.4f, 1.4f);
        this->setPos(x, y, z);
        this->modelName = "pig";
        this->heightOffset = 1.72f;
        this->textureName = "/mob/pig.png";
        this->deathScore = 10;
    }

    void die(Entity* e) override {
        int cnt = this->level->random->nextInt(2)+1;
        for (int i = 0; i < cnt; i++) {
            this->level->addEntity((Entity*)new Item(this->level, this->x, this->y, this->z, Tile::brownMushroom->id));
        }

        Mob::die(e);
    }
};