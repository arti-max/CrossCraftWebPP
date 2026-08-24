#include "item/TNT.hpp"
#include "CrossCraft.hpp"
#include "level/tile/Tile.hpp"
#include "item/Item.hpp"
#include "particle/SmokeParticle.hpp"
#include "particle/TileParticle.hpp"
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
    if (this->ticks < 40) {
        SmokeParticle* smoke = new SmokeParticle(this->level, this->x, this->y + 0.6f, this->z);
        CrossCraft::instance->particleEngine->add(smoke);
    }
    if (this->ticks >= 40) {
        float radius = 4.0f;
        this->level->explode(this, this->x, this->y, this->z, radius);
        for (int i = 0; i < 500; ++i) {
            float offsetX = (float)this->level->random->nextGaussian() * radius / 4.0f;
            float offsetY = (float)this->level->random->nextGaussian() * radius / 4.0f;
            float offsetZ = (float)this->level->random->nextGaussian() * radius / 4.0f;

            float distance = (float) std::sqrt((float)(offsetX*offsetX+offsetY*offsetY+offsetZ*offsetZ));

            float motionX = offsetX / distance / distance;
            float motionY = offsetY / distance / distance;
            float motionZ = offsetZ / distance / distance;

            this->level->cc->particleEngine->add(new TileParticle(this->level, this->x + offsetX, this->y + offsetY, this->z + offsetZ, motionX, motionY, motionZ, (Tile*)Tile::tnt));
        }
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
    glColor4f(1.0f, 1.0f, 1.0f, ((40-this->ticks + 1) % 2) * 0.4f);
    if (40 - this->ticks <= 16) {
        glColor4f(1.0f, 1.0f, 1.0f, ((40-this->ticks + 1) % 2) * 0.6f);
    }
    if (40 - this->ticks <= 4) {
        glColor4f(1.0f, 1.0f, 1.0f, 0.9f);
    }
    
    glPushMatrix();
    this->model->generateList();
    glPopMatrix();
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
        // this->remove();
        // this->level->addEntity(new Item(this->level, this->x, this->y, this->z, Tile::tnt->id));
    }
}

bool TNT::isPickable() {
    return true;
}