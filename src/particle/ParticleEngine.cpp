#include "particle/ParticleEngine.hpp"
#include <GL/gl.h>
#include <cmath>
#include <algorithm>

ParticleEngine::ParticleEngine(Level* level) : level(level) {}

ParticleEngine::~ParticleEngine() {
    for (Particle* p : particles) {
        delete p;
    }
    particles.clear();
}

void ParticleEngine::add(Particle* particle) {
    if (particle != nullptr) {
        particles.push_back(particle);
    }
}

void ParticleEngine::tick() {
    for (int i = particles.size() - 1; i >= 0; --i) {
        Particle* p = particles[i];
        p->tick();

        if (p->removed) {
            delete p;
            particles.erase(particles.begin() + i);
        }
    }
}

void ParticleEngine::render(Player* player, float partialTicks, int layer, Textures* textures) {
    Tessellator& t = Tessellator::getInstance();
    glEnable(GL_TEXTURE_2D);

    GLuint textureId = textures->loadTexture("terrain", GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, textureId);

    float cameraX = -std::cos(toRad(player->yRot));
    float cameraY = std::cos(toRad(player->xRot));
    float cameraZ = -std::sin(toRad(player->yRot));

    float cameraXWithY = -cameraZ * std::sin(toRad(player->xRot));
    float cameraZWithY = cameraX * std::sin(toRad(player->xRot));

    glColor4f(0.8f, 0.8f, 0.8f, 1.0f);

    t.begin();

    for (Particle* p : particles) {
        if (p->isLit() != (layer == 1)) {
            p->render(t, partialTicks, cameraX, cameraY, cameraZ, cameraXWithY, cameraZWithY);
        }
    }

    t.end();

    glDisable(GL_TEXTURE_2D);
}
