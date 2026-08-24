#pragma once
#include "particle/Particle.hpp"
#include "util/Random.hpp"

class WaterDropPatricle : public Particle {
public:
    WaterDropPatricle(Level* level, double x, double y, double z) : Particle(level, x, y, z, 0.0f, 0.0f, 0.0f) {
        this->xd *= 0.3f;
        this->yd = Random::random() * 0.2f + 0.1f;
        this->zd *= 0.3f;
        this->rCol = 1.0f;
        this->gCol = 1.0f;
        this->bCol = 1.0f;
        this->textureId = 16;
        this->setSize(0.01f, 0.01f);
        this->lifeTime = (int)(8.0f / (Random::random() * 0.8f + 0.2f));
    }

    void render(Tessellator& t, float partialTicks, float cameraX, float cameraY, float cameraZ, float cameraXWithY, float cameraZWithY) override {
        Particle::render(t, partialTicks, cameraX, cameraY, cameraZ, cameraXWithY, cameraZWithY);
    }

    void tick() override {
        this->xo = this->x;
        this->yo = this->y;
        this->zo = this->z;
        this->yd -= 0.06f;
        this->move(this->xd, this->yd, this->zd);
        this->xd *= 0.98f;
        this->yd *= 0.98f;
        this->zd *= 0.98f;
        if (this->lifeTime-- <= 0) {
            this->remove();
        }

        if (this->onGround) {
            if (Random::random() < 0.5f) {
                this->remove();
            }

            this->xd *= 0.7f;
            this->zd *= 0.7f;
        }
    }
};