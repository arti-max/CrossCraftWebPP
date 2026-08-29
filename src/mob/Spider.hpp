#pragma once
#include "level/Level.hpp"
#include "level/tile/Tile.hpp"
#include "item/Item.hpp"
#include "mob/Mob.hpp"
#include "mob/ai/JumpAttackAI.hpp"

class Spider : public Mob {
public:
    Spider(Level* level, float x, float y, float z) : Mob(level) {
        this->setSize(1.4f, 0.9f);
        this->setPos(x, y, z);
        this->heightOffset = 0.72f;
        this->modelName = "spider";
        this->ai = new JumpAttackAI();
        this->textureName = "/mob/spider.png";
    }

    void die(Entity* e) override {
        if (e != nullptr) {
            e->awardKillScore(this, 10);
        }
        // int cnt = this->level->random->nextInt(2)+1;
        // for (int i = 0; i < cnt; i++) {
        //     this->level->addEntity((Entity*)new Item(this->level, this->x, this->y, this->z, Tile::cobweb->id));
        // }

        Mob::die(e);
    }
};