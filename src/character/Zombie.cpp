#include "Zombie.hpp"
#include <GL/gl.h>
#include <cmath>
#include <cstdlib>
#include <chrono>
#include <random>

ZombieModel* Zombie::zombieModel = nullptr;

Zombie::Zombie(Level* level, Textures* textures, float x, float y, float z) : Entity(level) {
    this->textures = textures;
    
    if (!zombieModel) {
        initModel();
    }
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(0.0f, 1.0f);
    
    this->rotA = (dis(gen) + 1.0f) * 0.01f;
    this->setPos(x, y, z);
    this->timeOffs = dis(gen) * 1239813.0f;
    this->rot = dis(gen) * static_cast<float>(M_PI * 2.0);
    this->speed = 1.0f;
}

void Zombie::tick() {
    this->xo = this->x;
    this->yo = this->y;
    this->zo = this->z;
    
    float xa = 0.0f;
    float ya = 0.0f;
    
    if (this->y < -100.0f) {
        this->remove();
        return;
    }
    
    this->rot += this->rotA;
    this->rotA *= 0.99f;
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(-1.0f, 1.0f);
    
    float randomFactor = dis(gen) * dis(gen) * dis(gen) * dis(gen) * 0.08f;
    this->rotA += randomFactor;
    
    xa = std::sin(this->rot);
    ya = std::cos(this->rot);
    
    std::uniform_real_distribution<float> jumpDis(0.0f, 1.0f);
    if (this->onGround && jumpDis(gen) < 0.08) {
        this->yd = 0.5f;
    }
    
    this->moveRelative(xa, ya, this->onGround ? 0.1f : 0.02f);
    this->yd -= 0.08f;
    this->move(this->xd, this->yd, this->zd);
    
    this->xd *= 0.91f;
    this->yd *= 0.98f;
    this->zd *= 0.91f;
    
    if (this->onGround) {
        this->xd *= 0.7f;
        this->zd *= 0.7f;
    }
}

void Zombie::render(float partialTicks) {
glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, this->textures->loadTexture("char", GL_NEAREST));
    
    glPushMatrix();
    
    auto now = std::chrono::high_resolution_clock::now();
    auto duration = now.time_since_epoch();
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    double time = (static_cast<double>(milliseconds) / 1000.0 * 10.0 * static_cast<double>(this->speed)) + static_cast<double>(this->timeOffs);
    
    float interpX = this->xo + (this->x - this->xo) * partialTicks;
    float interpY = this->yo + (this->y - this->yo) * partialTicks;
    float interpZ = this->zo + (this->z - this->zo) * partialTicks;
    
    glTranslatef(interpX, interpY, interpZ);
    glScalef(1.0f, -1.0f, 1.0f);
    
    float size = 7.0f / 120.0f;
    glScalef(size, size, size);
    
    float offsetY = std::abs(std::sin(time * 2.0f / 3.0f)) * 5.0f + 24.5f;
    glTranslatef(0.0f, -offsetY, 0.0f);
    
    const float radToDeg = 180.0f / M_PI;
    glRotatef(this->rot * radToDeg + 180.0f, 0.0f, 1.0f, 0.0f);
    
    zombieModel->render(static_cast<float>(time));
    
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

void Zombie::initModel() {
    if (!zombieModel) {
        zombieModel = new ZombieModel();
    }
}

void Zombie::destroyModel() {
    if (zombieModel) {
        delete zombieModel;
        zombieModel = nullptr;
    }
}
