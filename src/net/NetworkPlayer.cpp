#include "net/NetworkPlayer.hpp"
#include <random>

NetworkPlayer::NetworkPlayer(Level* level, int var2, std::string &var3, float x, float y, float z, float yRot, float xRot) : Entity::Entity(level) {
    this->setPos(x, y, z);
    this->xRot = xRot;
    this->yRot = yRot;
    this->heightOffset = 1.62;

    this->movingTicks = 0;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(0.0f, 1.0f);
    this->timeOffs = dis(gen) * 1239813.0f;
    this->walkDist = 0.0f;
    this->walkDistO = 0.0f;

    this->serverX = x;
    this->serverY = y;
    this->serverZ = z;
    this->serverYaw = yRot;
    this->serverPitch = xRot;
}

NetworkPlayer::~NetworkPlayer() {
    delete model;
}   

void NetworkPlayer::tick() {
    this->xo = this->x;
    this->yo = this->y;
    this->zo = this->z;

    this->x += (this->serverX - this->x) * 0.5f;
    this->y += (this->serverY - this->y) * 0.5f;
    this->z += (this->serverZ - this->z) * 0.5f;
    this->yRot += (this->serverYaw - this->yRot) * 0.5f;
    this->xRot += (this->serverPitch - this->xRot) * 0.5f;

    float dx = this->x - this->xo;
    float dz = this->z - this->zo;
    
    if (std::sqrt(dx * dx + dz * dz) > 0.01f) {
        this->movingTicks = 6;
    } else if (this->movingTicks > 0) {
        this->movingTicks--;
    }
}

void NetworkPlayer::render(Textures* textures, float partialTicks) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textures->loadTexture("char", GL_NEAREST));
    glPushMatrix();
    float interpX = this->xo + (this->x - this->xo) * partialTicks;
    float interpY = this->yo + (this->y - this->yo) * partialTicks;
    float interpZ = this->zo + (this->z - this->zo) * partialTicks;
    float renderAnimTime = (float)(this->level->tickCount + partialTicks) * 0.4f;
    if (this->movingTicks == 0) {
        renderAnimTime = 0;
    }
    float bob = std::abs(std::sin(renderAnimTime * 0.6662f));
    float offsetY = -bob * 5.0f + 5.0f;
    float brightness = this->getBrightness();
    glColor3f(brightness, brightness, brightness);
    float scale = 0.058333334F;
    glTranslatef(interpX, interpY, interpZ);
    glScalef(1.0f, -1.0f, 1.0f);
    glScalef(scale, scale, scale);
    glTranslatef(0.0f, offsetY, 0.0f);
    glRotatef(-this->yRot, 0.0f, 1.0f, 0.0f);
    this->model->render((float)renderAnimTime, this->xRot);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

void NetworkPlayer::setServerPosition(float x, float y, float z, float yaw, float pitch) {
    this->serverX = x;
    this->serverY = y;
    this->serverZ = z;
    this->serverYaw = yaw;
    this->serverPitch = pitch;
}