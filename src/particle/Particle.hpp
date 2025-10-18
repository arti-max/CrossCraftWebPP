#pragma once
#include "Entity.hpp"
#include "level/Level.hpp"
#include "render/Tessellator.hpp"

class Particle : public Entity {
private:
    float textureUOffset;
    float textureVOffset;

    float size;
    int lifeTime;

    int age = 0;
public:
    int textureId;

    Particle(Level* level, double x, double y, double z, double motionX, double motionY, double motionZ, int textureId);
    void tick() override;
    void render(Tessellator& t, float partialTicks, float cameraX, float cameraY, float cameraZ, float cameraXWithY, float cameraZWithY);
};