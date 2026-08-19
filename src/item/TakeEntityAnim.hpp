#pragma once
#include "Entity.hpp"
#include "level/Level.hpp"
#include "render/Textures.hpp"

class TakeEntityAnim : public Entity {
private:
    int time = 0;
    Entity* item = nullptr;
    Entity* player = nullptr;

    float xorg = 0.0f;
    float yorg = 0.0f;
    float zorg = 0.0f;
public:
    TakeEntityAnim(Level* level, Entity* item, Entity* player) : Entity(level) {
        this->item = item;
        this->player = player;

        this->setSize(1.0f, 1.0f);

        this->xorg = item->x;
        this->yorg = item->y;
        this->zorg = item->z;

        this->x = this->xo = xorg;
        this->y = this->yo = yorg;
        this->z = this->zo = zorg;
        this->setPos(this->x, this->y, this->z);
    }

    ~TakeEntityAnim() override {
        if (item != nullptr) {
            delete item;
        } 
    }

    void tick() override {
        time++;

        if (time >= 3) {
            this->remove();
        }

        float dist = (float)time / 3.0f;
        dist = dist*dist;

        this->xo = item->xo = item->x;
        this->yo = item->yo = item->y;
        this->zo = item->zo = item->z;

        this->x = item->x = this->xorg + (player->x - this->xorg) * dist;
        this->y = item->y = this->yorg + (player->y - 1.0f - this->yorg) * dist;
        this->z = item->z = this->zorg + (player->z - this->zorg) * dist;
        this->setPos(this->x, this->y, this->z);

        item->xo = this->xo;
        item->yo = this->yo;
        item->zo = this->zo;
        item->x = this->x;
        item->y = this->y;
        item->z = this->z;
        item->setPos(this->x, this->y, this->z);
    }

    void render(float partialTicks, Textures* textures) override {
        if (!this->removed && this->item != nullptr) {
            this->item->render(partialTicks, textures);
        }
    }
};