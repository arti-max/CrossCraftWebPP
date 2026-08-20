#include "item/TNT.hpp"
#include "level/tile/Tile.hpp"
#include "item/Item.hpp"
#include <GL/gl.h>

TNT::TNT(Level* level, float x, float y, float z) : Entity(level) {
    this->setSize(1.15f, 1.15f);
    this->heightOffset = this->bbHeight / 2.0f;
    this->setPos(x+0.5f, y+0.5f, z+0.5f);
    this->yd = 0.3f;

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
    float interpX = this->xo + (this->x - this->xo) * partialTicks;
    float interpY = this->yo + (this->y - this->yo) * partialTicks;
    float interpZ = this->zo + (this->z - this->zo) * partialTicks;
    glTranslatef(interpX, interpY, interpZ);
    this->model->generateList();
    glPopMatrix();
}

void TNT::hurt(Entity* e, int dmg) {
    if (e->getEntityType() == EntityType::Player) {
        this->remove();
        this->level->addEntity(new Item(this->level, this->x, this->y, this->z, Tile::tnt->id));
    }
}