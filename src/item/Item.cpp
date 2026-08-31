#include "item/Item.hpp"
#include "util/Random.hpp"
#include "level/tile/Tile.hpp"
#include "util/Logger.hpp"
#include "player/Player.hpp"
#include "item/TakeEntityAnim.hpp"
#include "CrossCraft.hpp"

std::vector<ItemModel*> Item::models;

Item::Item(Level* level, float x, float y, float z, int resourceId) : Entity(level) {
    this->resourceId = resourceId;
    this->setSize(0.25f, 0.25f);
    this->heightOffset = this->bbHeight / 2.0f;
    this->setPos(x, y, z);
    this->yd = 0.2f;
    this->xd = (float)(Random::random() * 0.20000000298023224 - 0.10000000149011612);
    this->zd = (float)(Random::random() * 0.20000000298023224 - 0.10000000149011612);
    this->rot = (float)(Random::random() * 360.0);
    // Logger::logf(PREFIX_DEBUG, "Item spawned at: %f %f %f ID: %d\n", x, y, z, resourceId);
    this->makeStepSound = false;
}

void Item::tick() {
    this->xo = this->x;
    this->yo = this->y;
    this->zo = this->z;
    this->yd -= 0.04f;
    this->move(this->xd, this->yd, this->zd);
    this->xd *= 0.98f;
    this->yd *= 0.98f;
    this->zd *= 0.98f;
    if (this->onGround) {
        // Logger::logf(PREFIX_DEBUG, "ITEM: OnGround, bounce? %i\n", this->yd);
        this->xd *= 0.7f;
        this->zd *= 0.7f;
        this->yd *= -0.5f;
    }
    this->age++;
    this->hoverAnim++;
    if (this->age >= 6000) {
        this->remove();
    }
}

void Item::render(float partialTicks, Textures* textures) {
    Entity::render(partialTicks, textures);
    Tessellator& t = Tessellator::getInstance();
    glBindTexture(GL_TEXTURE_2D, textures->loadTexture("terrain", GL_NEAREST));
    float brightness = this->level->getBrightness((int)this->x, (int)this->y, (int)this->z);
    float rotation = this->rot + ((float)this->hoverAnim + partialTicks) * 3.0f;
    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
    glColor4f(brightness, brightness, brightness, 1.0f);
    float levitation = sin(this->hoverAnim / 10.0) * 0.1 + 0.1f;
    float interpX = this->xo + (this->x - this->xo) * partialTicks;
    float interpY = this->yo + (this->y - this->yo) * partialTicks;
    float interpZ = this->zo + (this->z - this->zo) * partialTicks;
    glTranslatef(interpX, interpY+levitation, interpZ);
    glRotatef(rotation, 0.0f, 1.0f, 0.0f);
    models[this->resourceId]->generateList();
    glDisable(GL_TEXTURE_2D);
    float glow;
    glow = (glow = (glow = std::sin(rotation / 10.0F) * 0.5F + 0.5F) * glow) * glow;

        glDisable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glDisable(GL_LIGHTING);
        glDisable(GL_ALPHA_TEST);
        
        glBlendFunc(GL_SRC_ALPHA, GL_ONE); 
        
        glDepthMask(GL_FALSE); 
        
        glColor4f(1.0f, 1.0f, 1.0f, glow * 0.4f); 
        glPushMatrix();
        
        models[this->resourceId]->generateList();
        
        glPopMatrix();
        
        glDepthMask(GL_TRUE);
        glEnable(GL_ALPHA_TEST);
        glEnable(GL_ALPHA_TEST);
        glDisable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glPopMatrix();
    glEnable(GL_TEXTURE_2D);
}

void Item::playerTouch(Player* player) {
    if (player->inventory->addItem(this->resourceId)) {
        this->removeExternally = true;
        this->level->addEntity(new TakeEntityAnim(this->level, this, player));
        this->remove();
    }
}

int Item::getResourceId() {
    return this->resourceId;
}

void Item::initModels() {
    if (!models.empty()) return;

    models.resize(256, nullptr);

    for (int t = 0; t < 256; ++t) {
        Tile* tile;
        if ((tile = Tile::tiles[t]) != nullptr) {
            Item::models[t] = new ItemModel(tile->textureId);
        }
    }
}