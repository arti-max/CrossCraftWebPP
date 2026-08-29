#include "particle/ParticleEngine.hpp"
#include <GL/gl.h>
#include <cmath>
#include <algorithm>

ParticleEngine::ParticleEngine(Level* level) : level(level) {
    this->particles.reserve(2);
    this->particles.resize(2);
}

ParticleEngine::~ParticleEngine() {
    for (int i = 0; i < 2; i++) {
        std::vector<Particle*> plist = this->particles[i];
        for (Particle* p : plist) {
            delete p;
        }
    }
    particles.clear();
}

void ParticleEngine::add(Particle* particle) {
    if (particle != nullptr) {
        int atlasId = particle->getParticleTexture();
        if (this->particles[atlasId].empty()) {
            this->particles[atlasId] = {};
        }
        particles[atlasId].push_back(particle);
    }
}

void ParticleEngine::tick() {
    for (int j = 0; j < 2; j++) {
        for (int i = particles[j].size() - 1; i >= 0; --i) {
            Particle* p = this->particles[j][i];
            p->tick();

            if (p->removed) {
                delete p;
                particles[j].erase(particles[j].begin() + i);
            }
        }
    }
}

void ParticleEngine::render(Player* player, float partialTicks, Textures* textures) {
    if (particles.empty()) {
        return;
    }

    Tessellator& t = Tessellator::getInstance();
    glEnable(GL_TEXTURE_2D);

    float cameraX = -std::cos(toRad(player->yRot));
    float cameraY = std::cos(toRad(player->xRot));
    float cameraZ = -std::sin(toRad(player->yRot));

    float cameraXWithY = -cameraZ * std::sin(toRad(player->xRot));
    float cameraZWithY = cameraX * std::sin(toRad(player->xRot));

    for (int i = 0; i < 2; i++) {
        GLuint textureId = 0;
        if (i == 1) {
            textureId = textures->loadTexture("terrain", GL_NEAREST);
        }
        if (i == 0) {
            textureId = textures->loadTexture("/particles.png", GL_NEAREST);
        }
        glBindTexture(GL_TEXTURE_2D, textureId);

        t.begin();

        for (Particle* p : particles[i]) {
            float brightness = p->getBrightness(partialTicks) * 0.8f;
            t.color(brightness, brightness, brightness);
            p->render(t, partialTicks, cameraX, cameraY, cameraZ, cameraXWithY, cameraZWithY);
        }

        t.end();
    }

    glDisable(GL_TEXTURE_2D);
}
