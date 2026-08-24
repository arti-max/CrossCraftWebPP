#pragma once
#include "particle/Particle.hpp"
#include "util/Random.hpp"

class SmokeParticle : public Particle {

public:
    SmokeParticle(Level* level, double x, double y, double z) : Particle(level, x, y, z, 0.0f, 0.0f, 0.0f) {
        this->xd *= 0.1f;
        this->yd *= 0.1f;
        this->zd *= 0.1f;
        this->rCol = this->gCol = this->bCol = (float)(Random::random() * 0.30000001192092896f);
        this->lifeTime = (int)(8.0f / (Random::random() * 0.8f + 0.2f));
        this->noPhysics = true;
    }

    void tick() override {
        this->xo = this->x;
        this->yo = this->y;
        this->zo = this->z;
        if (this->age++ >= this->lifeTime) {
            this->remove();
        }

        this->textureId = 7 - (this->age << 3) / this->lifeTime;
        this->yd = (float)(this->yd + 0.004f);
        this->move(this->xd, this->yd, this->zd);
        this->xd *= 0.96f;
        this->yd *= 0.96f;
        this->zd *= 0.96f;
        if (this->onGround) {
            this->xd *= 0.7f;
            this->zd *= 0.7f;
        }
    }
};