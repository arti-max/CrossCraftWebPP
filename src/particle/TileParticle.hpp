#pragma once
#include "particle/Particle.hpp"
#include "level/tile/Tile.hpp"

class TileParticle : public Particle {
public:
    TileParticle(Level* level, float x, float y, float z, float motionX, float motionY, float motionZ, Tile* tile) : Particle(level, x, y, z, motionX, motionY, motionZ) {
        this->textureId = tile->textureId;
        this->gravity = tile->particleGravity;
        this->rCol = 0.6f;
        this->gCol = 0.6f;
        this->bCol = 0.6f;
    }

    int getParticleTexture() override {
        return 1;
    }

    void render(Tessellator& t, float partialTicks, float cameraX, float cameraY, float cameraZ, float cameraXWithY, float cameraZWithY) override {
        float minU = (this->textureId % 16 + this->textureUOffset / 4.0F) / 16.0F;
        float maxU = minU + 999.0F / 64000.0F;
        float minV = ((float) (this->textureId / 16) + this->textureVOffset / 4.0F) / 16.0F;
        float maxV = minV + 999.0F / 64000.0F;

        float x = this->xo + (this->x - this->xo) * partialTicks;
        float y = this->yo + (this->y - this->yo) * partialTicks;
        float z = this->zo + (this->z - this->zo) * partialTicks;

        float brightness = this->getBrightness(partialTicks);

        float size = this->size * 0.1f;
        t.color(brightness, brightness, brightness);
        t.vertexUV(x - cameraX * size - cameraXWithY * size, y - cameraY * size, z - cameraZ * size - cameraZWithY * size, minU, maxV);
        t.vertexUV(x - cameraX * size + cameraXWithY * size, y + cameraY * size, z - cameraZ * size + cameraZWithY * size, minU, minV);
        t.vertexUV(x + cameraX * size + cameraXWithY * size, y + cameraY * size, z + cameraZ * size + cameraZWithY * size, maxU, minV);
        t.vertexUV(x + cameraX * size - cameraXWithY * size, y - cameraY * size, z + cameraZ * size - cameraZWithY * size, maxU, maxV);
    }
};