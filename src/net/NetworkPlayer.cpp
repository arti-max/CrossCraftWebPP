#include "net/NetworkPlayer.hpp"

NetworkPlayer::NetworkPlayer(Level* level, int var2, std::string &var3, float x, float y, float z, float yRot, float xRot) : Entity::Entity(level) {
    this->setPos(x, y, z);
    this->xRot = xRot;
    this->yRot = yRot;
    this->heightOffset = 1.62;
}

void NetworkPlayer::tick() {
    this->xo = this->x;
    this->yo = this->y;
    this->zo = this->z;
    ++this->ticks;
}

void NetworkPlayer::render(Textures* textures, float partialTicks) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textures->loadTexture("char", GL_NEAREST));
    glPushMatrix();
    float smooth = ((float)this->ticks + partialTicks) / 2.0f;
    float brightness = this->getBrightness();
    glColor3f(brightness, brightness, brightness);
    float v1 = (float)(-std::abs(std::sin((double)smooth * 0.6662f)) * 5.0f - 23.0f);
    float v2 = 0.058333334F;
    glTranslatef(this->xo + (this->x - this->xo) * partialTicks, this->yo + (this->y - this->yo) * partialTicks, this->zo + (this->z - this->zo) * partialTicks);
    glScalef(1.0f, -1.0f, 1.0f);
    glScalef(v2, v2, v2);
    glTranslatef(0.0f, v1, 0.0f);
    glRotatef(-this->yRot, 0.0f, 1.0f, 0.0f);
    this->model->render(partialTicks);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}