#include "item/Sign.hpp"
#include "CrossCraft.hpp"
#include "render/Textures.hpp"
#include "gui/Font.hpp"
#include "item/SignModel.hpp"

Sign::Sign(CrossCraft* cc, float x, float y, float z, float rot) : Entity(cc->level) {
    this->setSize(0.5f, 1.5f);
    this->heightOffset = this->bbHeight / 2.0f;
    this->setPos(x, y, z);
    this->font = cc->font;
    this->rot = -rot;
    this->heightOffset = 1.5f;
    this->xd = -(std::sin(this->rot * M_PI / 180.0f)) * 0.05f;
    this->yd = 0.2f;
    this->zd = -(std::cos(this->rot * M_PI / 180.0f)) * 0.05f;
    this->makeStepSound = false;
    this->messages.resize(4);
    this->messages = {"This is a test", "of the signs.", "Each line can", "be 15 chars!"};
}

bool Sign::isPickable() {
    return !this->removed;
}

void Sign::tick() {
    this->xo = this->x;
    this->yo = this->y;
    this->zo = this->z;
    this->yd -= 0.04f;
    this->move(this->xd, this->yd, this->zd);
    this->xd *= 0.98f;
    this->yd *= 0.98f;
    this->zd *= 0.98f;
    if (this->onGround) {
        this->xd *= 0.7f;
        this->zd *= 0.7f;
        this->yd *= -0.5f;
    }
}

void Sign::render(float partialTicks, Textures* textures) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textures->loadTexture("/item/sign.png", GL_NEAREST));
    float bright = this->level->getBrightness((int)this->x, (int)this->y, (int)this->z);
    glPushMatrix();
    glColor4f(bright, bright, bright, 1.0f);
    float intX = this->xo + (this->x - this->xo) * partialTicks;
    float intY = this->yo + (this->y - this->yo) * partialTicks;
    float intZ = this->zo + (this->z - this->zo) * partialTicks;
    glTranslatef(intX, intY - this->heightOffset / 2.0f, intZ);
    glRotatef(this->rot, 0.0f, 1.0f, 0.0f);
    glPushMatrix();
    glScalef(1.0f, -1.0f, -1.0f);
    SignModel* model = new SignModel();
    model->render(0.0625f);
    glPopMatrix();
    float scale = 0.016666668f;
    glTranslatef(0.0f, 0.5f, 0.09f);
    glScalef(scale, -scale, scale);
    glNormal3f(0.0f, 0.0f, -1.0f * scale);
    glEnable(GL_BLEND);
    glDisable(GL_CULL_FACE);
    for (int msg = 0; msg < 4; ++msg) {
        std::string message = this->messages[msg];
        this->font->draw(message, -this->font->width(message) / 2, msg * 10 - 4 * 5, 0x00202020);
    }
    glEnable(GL_CULL_FACE);
    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glPopMatrix();
    delete model;
}