#include "item/TNT.hpp"
#include "level/tile/Tile.hpp"
#include "item/Item.hpp"
#include <GL/gl.h>

TNT::TNT(Level* level, float x, float y, float z) : Entity(level) {
    this->setSize(1.15f, 1.15f);
    this->heightOffset = this->bbHeight / 2.0f;
    this->setPos(x+0.5f, y+0.5f, z+0.5f);
    this->yd = 0.3f;
    this->xd = (float)(Random::random() * 0.20000000298023224 - 0.10000000149011612) * 0.1;
    this->zd = (float)(Random::random() * 0.20000000298023224 - 0.10000000149011612) * 0.1;

    this->model = new TNTModel(8);
}

void TNT::tick() {
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
    }
    this->ticks++;
    if (this->ticks >= 60) {
        this->level->explode(this, this->x, this->y, this->z, 4.0f);
        this->remove();
    }
}

void TNT::render(float partialTicks, Textures* textures) {
    Entity::render(partialTicks, textures);
    glBindTexture(GL_TEXTURE_2D, textures->loadTexture("terrain", GL_NEAREST));
    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
    float brightness = this->level->getBrightness((int)this->x, (int)this->y, (int)this->z);
    float interpX = this->xo + (this->x - this->xo) * partialTicks;
    float interpY = this->yo + (this->y - this->yo) * partialTicks;
    float interpZ = this->zo + (this->z - this->zo) * partialTicks;
    glTranslatef(interpX, interpY, interpZ);
    glColor4f(brightness, brightness, brightness, 1.0f);
    this->model->generateList();
    float br = (float)std::sin((double)emscripten_get_now() / 100.0f) * 0.2f + 0.8f;
    glDisable(GL_TEXTURE_2D);

    glEnable(GL_BLEND);
    glDisable(GL_LIGHTING);
    glDisable(GL_ALPHA_TEST);
    
    glBlendFunc(GL_SRC_ALPHA, GL_ONE); 
    
    glDepthMask(GL_FALSE);
    glColor4f(1.0f, 1.0f, 1.0f, br * 0.4f);
    if (br >= 0.7f) {
        glPushMatrix();
        this->model->generateList();
        glPopMatrix();
    }
    glDepthMask(GL_TRUE);
    glEnable(GL_ALPHA_TEST);
    glEnable(GL_ALPHA_TEST);
    glDisable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);

    glPopMatrix();
}

void TNT::hurt(Entity* e, int dmg) {
    if (e->getEntityType() == EntityType::Player && dmg >= 4) {
        this->remove();
        this->level->addEntity(new Item(this->level, this->x, this->y, this->z, Tile::tnt->id));
    }
}

bool TNT::isPickable() {
    return true;
}