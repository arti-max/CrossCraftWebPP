#pragma once
#include "Entity.hpp"
#include "render/Textures.hpp"
#include "render/Tessellator.hpp"
#include "item/TNTModel.hpp"

class TNT : public Entity {
private:
    int ticks = 0;

    float xd = 0.0f;
    float yd = 0.0f;
    float zd = 0.0f;

    TNTModel* model = nullptr;
public:
    TNT(Level* level,  float x, float y, float z);
    void tick() override;
    void render(float partialTicks, Textures* textures) override;
    void hurt(Entity* e, int dmg) override;
    bool isPickable() override;
};