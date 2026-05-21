#pragma once
#include "Entity.hpp"
#include "level/Level.hpp"
#include "render/Tessellator.hpp"

class Tile;

class Particle : public Entity {
protected:
    float rCol = 1.0f;
    float gCol = 1.0f;
    float bCol = 1.0f;
    float textureUOffset = 0.0f;
    float textureVOffset = 0.0f;

    float size = 1.0f;
    int lifeTime = 0.0f;

    int age = 0;
public:
    int textureId = 0;
    float gravity = 0.04f;

    Particle(Level* level, double x, double y, double z, double motionX, double motionY, double motionZ);
    virtual Particle* setPower(float power);
    virtual Particle* setScale(float scale);
    void tick() override;
    virtual void render(Tessellator& t, float partialTicks, float cameraX, float cameraY, float cameraZ, float cameraXWithY, float cameraZWithY);
    virtual int getParticleTexture();
};