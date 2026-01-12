#pragma once
#include "Entity.hpp"
#include "level/Level.hpp"
#include "render/Tessellator.hpp"

class Tile;

class Particle : public Entity {
protected:
    float rCol;
    float gCol;
    float bCol;
private:
    float textureUOffset;
    float textureVOffset;

    float size;
    int lifeTime;

    int age = 0;
public:
    int textureId;
    float gravity = 0.04f;

    Particle(Level* level, double x, double y, double z, double motionX, double motionY, double motionZ, Tile* tile);
    virtual Particle* setPower(float power);
    virtual Particle* setScale(float scale);
    void tick() override;
    void render(Tessellator& t, float partialTicks, float cameraX, float cameraY, float cameraZ, float cameraXWithY, float cameraZWithY);
};