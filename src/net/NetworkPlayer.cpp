#include "net/NetworkPlayer.hpp"
#include <random>

float normalizeAngle(float angle) {
    while (angle >= 180.0f) angle -= 360.0f;
    while (angle < -180.0f) angle += 360.0f;
    return angle;
}

NetworkPlayer::NetworkPlayer(Level* level, int playerId, std::string &username, float x, float y, float z, float yRot, float xRot) : Entity::Entity(level) {
    this->playerId = playerId;
    this->username = username;
    this->displayName = username;
    
    this->setPos(x, y, z);
    this->xRot = xRot;
    this->yRot = yRot;
    this->heightOffset = 1.62f;
    
    this->yBodyRot = yRot;
    this->yBodyRotO = yRot;
    
    this->animStep = 0.0f;
    this->animStepO = 0.0f;
    this->run = 0.0f;
    this->oRun = 0.0f;
}

NetworkPlayer::~NetworkPlayer() {
    delete model;
}   

void NetworkPlayer::tick() {
    Entity::tick(); 
    
    this->animStepO = this->animStep;
    
    int i = 5;
    do {
        if (this->moveQueue.size() > 0) {
            NetworkPosition pos = this->moveQueue.front();
            this->moveQueue.pop_front();
            this->setPos(pos.x, pos.y, pos.z);
            this->yRot = pos.yaw;
            this->xRot = pos.pitch;
        }
    } while (i-- > 0 && this->moveQueue.size() > 10);

    float dx = this->x - this->xo;
    float dz = this->z - this->zo;
    
    this->yBodyRotO = this->yBodyRot;
    
    float dist = std::sqrt(dx * dx + dz * dz);
    float targetBodyRot = this->yBodyRot;
    float speed = 0.0f;
    
    this->oRun = this->run;
    float runSpeed = 0.0f;

    if (dist == 0.0f) {
        this->animStep = 0.0f;
    } else {
        runSpeed = 1.0f;
        speed = dist * 3.0f;
        targetBodyRot = -((float)std::atan2(dz, dx) * 180.0f / 3.1415927f + 90.0f);
    }

    this->run += (runSpeed - this->run) * 0.1f;

    float rotDiff = targetBodyRot - this->yBodyRot;
    while (rotDiff < -180.0f) rotDiff += 360.0f;
    while (rotDiff >= 180.0f) rotDiff -= 360.0f;
    
    this->yBodyRot += rotDiff * 0.1f;

    float headDiff = this->yRot - this->yBodyRot;
    while (headDiff < -180.0f) headDiff += 360.0f;
    while (headDiff >= 180.0f) headDiff -= 360.0f;
    
    this->yBodyRot += headDiff * 0.1f;
    
    headDiff = this->yRot - this->yBodyRot;
    while (headDiff < -180.0f) headDiff += 360.0f;
    while (headDiff >= 180.0f) headDiff -= 360.0f;
    
    bool isMovingBackwards = headDiff < -90.0f || headDiff >= 90.0f;
    
    if (headDiff < -75.0f) headDiff = -75.0f;
    if (headDiff >= 75.0f) headDiff = 75.0f;
    
    this->yBodyRot = this->yRot - headDiff;
    
    if (isMovingBackwards) {
        speed = -speed;
    }

    while (this->yRot - this->yRotO < -180.0f) this->yRotO -= 360.0f;
    while (this->yRot - this->yRotO >= 180.0f) this->yRotO += 360.0f;
    
    while (this->yBodyRot - this->yBodyRotO < -180.0f) this->yBodyRotO -= 360.0f;
    while (this->yBodyRot - this->yBodyRotO >= 180.0f) this->yBodyRotO += 360.0f;
    
    while (this->xRot - this->xRotO < -180.0f) this->xRotO -= 360.0f;
    while (this->xRot - this->xRotO >= 180.0f) this->xRotO += 360.0f;

    this->animStep += speed;
}

void NetworkPlayer::render(Textures* textures, float partialTicks, Font* font, Player* localPlayer) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textures->loadTextureFromUrl("http://crosscraftweb.ddns.net/skins/" + this->username + ".png", GL_NEAREST));

    float interpX = this->xo + (this->x - this->xo) * partialTicks;
    float interpY = this->yo + (this->y - this->yo) * partialTicks;
    float interpZ = this->zo + (this->z - this->zo) * partialTicks;
    
    float interpYBodyRot = this->yBodyRotO + (this->yBodyRot - this->yBodyRotO) * partialTicks;
    float interpYHeadRot = this->yRotO + (this->yRot - this->yRotO) * partialTicks;
    float interpXRot = this->xRotO + (this->xRot - this->xRotO) * partialTicks;
    
    float interpRun = this->oRun + (this->run - this->oRun) * partialTicks;
    float interpAnimStep = this->animStepO + (this->animStep - this->animStepO) * partialTicks;
    float modelHeadRot = interpYHeadRot - interpYBodyRot;

    glPushMatrix();
    float brightness = this->getBrightness();
    glColor3f(brightness, brightness, brightness);
    float bob = -std::abs(std::sin(interpAnimStep * 0.6662f)) * 5.0f * interpRun - 23.0f;
    
    glTranslatef(interpX, interpY - this->heightOffset, interpZ);
    glScalef(1.0f, -1.0f, 1.0f);
    glScalef(0.0625f, 0.0625f, 0.0625f);
    
    glTranslatef(0.0f, bob, 0.0f);
    glRotatef(-interpYBodyRot, 0.0f, 1.0f, 0.0f);
    this->model->render(interpAnimStep, -modelHeadRot, interpXRot); 

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

void NetworkPlayer::queue(float x, float y, float z, float yaw, float pitch) {
    this->moveQueue.push_back(NetworkPosition(x, y, z, yaw, pitch));
}

void NetworkPlayer::queue(float x, float y, float z) {
    this->moveQueue.push_back(NetworkPosition(x, y, z, this->yRot, this->xRot));
}

void NetworkPlayer::queue(float yaw, float pitch) {
    this->moveQueue.push_back(NetworkPosition(this->x, this->y, this->z, yaw, pitch));
}

// void NetworkPlayer::setServerPosition(float x, float y, float z, float yaw, float pitch) {
//     this->serverX = x;
//     this->serverY = y;
//     this->serverZ = z;
//     this->serverYaw = yaw;
//     this->serverPitch = pitch;
// }