#include "net/NetworkPlayer.hpp"
#include <random>

NetworkPlayer::NetworkPlayer(Level* level, int playerId, std::string &username, float x, float y, float z, float yRot, float xRot) : Entity::Entity(level) {
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

    this->username = username;
    this->playerId = playerId;
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

void NetworkPlayer::render(Textures* textures, float partialTicks, Font* font, Player* localPlayer) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textures->loadTexture("char", GL_NEAREST));
    glPushMatrix();
    float interpX = this->xo + (this->x - this->xo) * partialTicks;
    float interpY = this->yo + (this->y - this->yo) * partialTicks;
    float interpZ = this->zo + (this->z - this->zo) * partialTicks;
    float renderAnimTime = (float)(this->level->tickCount + partialTicks) * 0.5f;
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

    float dx = interpX - localPlayer->x;
    float dy = interpY - localPlayer->y;
    float dz = interpZ - localPlayer->z;
    float distanceSq = dx*dx + dy*dy + dz*dz;

    if (distanceSq < 4096.0f) {
        glPushMatrix();
        
        glTranslatef(interpX, interpY + 0.8f, interpZ);

        glRotatef(-localPlayer->yRot, 0.0f, 1.0f, 0.0f);
        glRotatef(-localPlayer->xRot, 1.0f, 0.0f, 0.0f);

        float scale = 0.005f * std::sqrt(distanceSq);
        const float min_scale = 0.02f;
        const float max_scale = 0.2f;

        if (scale < min_scale) scale = min_scale;
        if (scale > max_scale) scale = max_scale;
        
        glScalef(scale, -scale, scale); 

        glDisable(GL_BLEND);
        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_FOG);
        
        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
        font->drawCentered(this->username, 0, 0, 0xFFFFFF);

        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        glDisable(GL_DEPTH_TEST);
        font->drawCentered(this->username, 0, 0, 0xFFFFFF);
        glEnable(GL_DEPTH_TEST);

        glEnable(GL_BLEND);
        glEnable(GL_LIGHTING);
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_FOG);

        glPopMatrix();
    }
    glDisable(GL_TEXTURE_2D);
}

void NetworkPlayer::setServerPosition(float x, float y, float z, float yaw, float pitch) {
    this->serverX = x;
    this->serverY = y;
    this->serverZ = z;
    this->serverYaw = yaw;
    this->serverPitch = pitch;
}