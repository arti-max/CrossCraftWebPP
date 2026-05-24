#include "item/Arrow.hpp"
#include "CrossCraft.hpp"
#include "render/Textures.hpp"
#include "level/EntityMesh.hpp"

Arrow::Arrow(CrossCraft* cc, Player* player, float x, float y, float z, float yaw, float pitch) : Entity(cc->level) {
    this->owner = player;
    this->setSize(0.25f, 0.5f);
    // this->heightOffset = this->bbHeight / 2.0f;
    this->heightOffset = 0.25f;

    float cosYaw = std::cos((-yaw) * M_PI / 180.0f - M_PI);
    float sinYaw = std::sin((-yaw) * M_PI / 180.0f - M_PI);
    float cosPitch = std::cos((-pitch) * M_PI / 180.0f);
    float sinPitch = std::sin((-pitch) * M_PI / 180.0f);

    this->slide = false;
    float speed = 0.8f;

    this->xo -= cosYaw * 0.2f;
    this->zo += sinYaw * 0.2f;
    x -= cosYaw * 0.2f;
    z += sinYaw * 0.2f;

    this->xd = sinYaw * cosPitch * speed;
    this->yd = sinPitch * speed;
    this->zd = cosYaw * cosPitch * speed;

    this->setPos(x, y, z);

    float dist = std::sqrt(this->xd*this->xd+this->zd*this->zd);
    this->yRotO = this->yRot = std::atan2(this->xd, this->zd) * 180.0f / M_PI;
    this->xRotO = this->xRot = std::atan2(this->yd, dist) * 180.0f / M_PI;

    this->makeStepSound = false;
}

void Arrow::tick() {
    ++this->time;
    this->xRotO = this->xRot;
    this->yRotO = this->yRot;
    this->xo = this->x;
    this->yo = this->y;
    this->zo = this->z;

    if (this->hasHit) {
        ++this->stickTime;
        if (this->stickTime >= 20) {
            this->remove();
            return;
        }
    } else {
        this->xd *= 0.992f;
        this->yd *= 0.992f;
        this->zd *= 0.992f;
        this->yd -= 0.02f;

        int steps = std::sqrt(this->xd*this->xd + this->yd*this->yd + this->zd*this->zd) / 2.0f + 1.0f;
        float sx = this->xd / steps;
        float sy = this->yd / steps;
        float sz = this->zd / steps;

        for (int i = 0; i < steps && !this->collision; ++i) {
            AABB bbox = this->bb.expand(sx, sy, sz);

            if (this->level->getCubes(bbox).size() > 0) {
                this->collision = true;
            }
            if (this->level != nullptr) {
                std::vector<Entity*> entities = this->level->emesh->getEntities(this, bbox);
                Entity* e = nullptr;
                for (int i = 0; i<entities.size(); ++i) {
                    e = entities[i];
                    if (e->isShootable() && e->getEntityType() != this->owner->getEntityType() && this->time <= 20) {
                        e->hurt(this, 3);
                        this->collision = true;
                        this->stickTime = 20;
                    }
                }

                if (!this->collision) {
                    this->bb.move(sx, sy, sz);
                    this->x += sx;
                    this->y += sy;
                    this->z += sz;
                }
            }
        }

        if (this->collision) {
            this->hasHit = true;
            this->xd = this->yd = this->zd = 0.0f;
        }

        if (!this->hasHit) {
            float dist = std::sqrt(this->xd*this->xd + this->zd*this->zd);
            this->xRot = std::atan2(this->xd, this->zd) * 180.0f / M_PI;
            this->xRot = std::atan2(this->yd, dist) * 180.0f / M_PI;

            while (this->xRot - this->xRotO < -180.0F) this->xRotO -= 360.0F;
            while (this->xRot - this->xRotO >= 180.0F) this->xRotO += 360.0F;
            while (this->yRot - this->yRotO < -180.0F) this->yRotO -= 360.0F;
            while (this->yRot - this->yRotO >= 180.0F) this->yRotO += 360.0F;
        }
    }
}

void Arrow::render(float partialTicks, Textures* textures) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textures->loadTexture("/item/arrows.png", GL_NEAREST));

    float brightness = this->level->getBrightness((int)this->x, (int)this->y, (int)this->z);
    glPushMatrix();
    glColor4f(brightness, brightness, brightness, 1.0f);

    float intX = this->xo + (this->x - this->xo) * partialTicks;
    float intY = this->yo + (this->y - this->yo) * partialTicks;
    float intZ = this->zo + (this->z - this->zo) * partialTicks;
    glTranslatef(intX, intY-this->heightOffset/2.0f, intZ);

    float iyaw = this->yRotO + (this->yRot - this->yRotO) * partialTicks - 90.0f;
    float ipitch = this->xRotO + (this->xRot - this->xRotO) * partialTicks;
    glRotatef(iyaw, 0.0f, 1.0f, 0.0f);
    glRotatef(ipitch, 0.0f, 0.0f, 1.0f);
    glRotatef(45.0f, 1.0f, 0.0f, 0.0f);

    Tessellator& t = Tessellator::getInstance();
    float texU = 0.5f;
    float texV = 0.15625f;
    float scale = 0.05625f;
    glScalef(scale, scale, scale);

    for (int f = 0; f < 4; ++f) {
        glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
        glNormal3f(0.0f, -scale, 0.0f);
        t.begin();
        t.vertexUV(-8.0f, -2.0f, 0.0f, 0.0f, 0.0f);
        t.vertexUV(8.0f, -2.0f, 0.0f, texU, 0.0f);
        t.vertexUV(8.0f, 2.0f, 0.0f, texU, texV);
        t.vertexUV(-8.0f, 2.0f, 0.0f, 0.0f, texV);
        t.end();
    }

    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glPopMatrix();
}

void Arrow::awardKillScore(Entity* e, int score) {
    this->owner->awardKillScore(e, score);
}