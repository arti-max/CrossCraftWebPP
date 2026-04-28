#pragma once
#include "Entity.hpp"
#include "Level.hpp"
#include "render/Textures.hpp"

class TakeEntityAnim : public Entity {
private:
    int time = 0;
    Entity* item;
    Entity* player;

    float xorg;
    float yorg;
    float zorg;
public:
    TakeEntityAnim(Level* level, Entity* item, Entity* player) : Entity(level) {
        this->item = item;
        this->player = player;

        this->setSize(1.0f, 1.0f);

        xorg = item->x;
        yorg = item->y;
        zorg = item->z;
    }

    void tick() override {
        time++;

        if (time >= 3) {
            this->remove();
        }

        float dist = (dist = (float)time / 3.0f) * dist;

        xo = item->xo = item->x;
        yo = item->yo = item->y;
        zo = item->zo = item->z;

        x = item->x = xorg + (player->x - xorg) * dist;
        y = item->y = yorg + (player->y - 1.0f - yorg) * dist;
        z = item->z = zorg + (player->z - zorg) * dist;

        this->setPos(x, y, z);
    }

    void render(float partialTicks, Textures* textures) override {
        this->item->render(partialTicks, textures);
    }
};