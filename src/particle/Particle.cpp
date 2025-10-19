#include "particle/Particle.hpp"
#include <cstdlib>
#include <ctime>

double randomDouble() {
    return (double)rand() / RAND_MAX;
}

Particle::Particle(Level* level, double x, double y, double z, double motionX, double motionY, double motionZ, int textureId) : Entity::Entity(level) {
    this->textureId = textureId;

    this->setSize(0.2f, 0.2f);
    this->heightOffset = this->bbHeight / 2.0f;

    this->setPos(x, y, z);

    this->xd = motionX + (randomDouble() * 2.0f - 1.0f) * 0.4f;
    this->yd = motionY + (randomDouble() * 2.0f - 1.0f) * 0.4f;
    this->zd = motionZ + (randomDouble() * 2.0f - 1.0f) * 0.4f;

    double speed = (randomDouble() + randomDouble() + 1.0f) * 0.15f;

    double distance = std::sqrt(this->xd * this->xd + this->yd * this->yd + this->zd * this->zd);
    this->xd = this->xd / distance * speed * 0.7f;
    this->yd = this->yd / distance * speed;
    this->zd = this->zd / distance * speed * 0.7f;

    this->textureUOffset = randomDouble() * 3.0f;
    this->textureVOffset = randomDouble() * 3.0f;

    this->size = randomDouble() * 0.5f + 0.5f;
    this->lifeTime = (int) (4.0f / (randomDouble() * 0.9f + 0.1f));
}

void Particle::tick() {
    Entity::tick();

    if (this->age++ >= this->lifeTime) {
        this->remove();
    }

    this->yd -= 0.06f;

    this->move(this->xd, this->yd, this->zd);

    this->xd *= 0.98f;
    this->yd *= 0.98f;
    this->zd *= 0.98f;

    if (this->onGround) {
        this->xd *= 0.7f;
        this->zd *= 0.7f;
    }
}

void Particle::render(Tessellator& t, float partialTicks, float cameraX, float cameraY, float cameraZ, float cameraXWithY, float cameraZWithY) {
    float minU = (this->textureId % 16 + this->textureUOffset / 4.0F) / 16.0F;
    float maxU = minU + 999.0F / 64000.0F;
    float minV = ((float) (this->textureId / 16) + this->textureVOffset / 4.0F) / 16.0F;
    float maxV = minV + 999.0F / 64000.0F;

    float x = this->xo + (this->x - this->xo) * partialTicks;
    float y = this->yo + (this->y - this->yo) * partialTicks;
    float z = this->zo + (this->z - this->zo) * partialTicks;

    float size = this->size * 0.1f;
    
    t.vertexUV(x - cameraX * size - cameraXWithY * size, y - cameraY * size, z - cameraZ * size - cameraZWithY * size, minU, maxV);
    t.vertexUV(x - cameraX * size + cameraXWithY * size, y + cameraY * size, z - cameraZ * size + cameraZWithY * size, minU, minV);
    t.vertexUV(x + cameraX * size + cameraXWithY * size, y + cameraY * size, z + cameraZ * size + cameraZWithY * size, maxU, minV);
    t.vertexUV(x + cameraX * size - cameraXWithY * size, y - cameraY * size, z + cameraZ * size - cameraZWithY * size, maxU, maxV);
}