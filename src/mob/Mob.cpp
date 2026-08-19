#include "mob/Mob.hpp"
#include "level/Level.hpp"
#include "render/Textures.hpp"
#include "ai/BasicAI.hpp"
#include "model/ModelManager.hpp"

ModelManager* Mob::modelManager = new ModelManager();

Mob::Mob(Level* level) : Entity(level) {
    this->x = 0.0f;
    this->y = 0.0f;
    this->z = 0.0f;
    this->setPos(this->x, this->y, this->z);
    this->timeOffs = Random::random() * 12398.0f;
    this->rot = Random::random() * M_PI * 2.0f;
    this->speed = 1.0f;
    this->ai = new BasicAI();
}

Mob::~Mob() {
    if (this->ai != nullptr) {
        delete this->ai;
    }
}

bool Mob::isPushable() {
    return !this->removed;
}

bool Mob::isPickable() {
    return !this->removed;
}

void Mob::tick() {
    Entity::tick();
    this->oTilt = this->tilt;
    if (this->attackTime > 0) {
        --this->attackTime;
    }

    if (this->hurtTime > 0) {
        --this->hurtTime;
    }

    if (this->invulnerableTime > 0) {
        --this->invulnerableTime;
    }

    if (this->health <= 0) {
        ++this->deathTime;
        if (this->deathTime > 20) {
            if (this->ai != nullptr) {
                this->ai->beforeRemove();
            }

            this->remove();
        }
    }

    if (this->isUnderWater()) {
        if (this->airSupply > 0) {
            --this->airSupply;
        } else {
            this->hurt(nullptr, 2);
        }
    } else {
        this->airSupply = Mob::TOTAL_AIR_SUPPLY;
    }

    if (this->isInWater()) {
        this->fallDistance = 0.0f;
    }

    if (this->isInLava()) {
        this->hurt(nullptr, 10);
    }

    this->animStepO = this->animStep;
    this->yBodyRotO = this->yBodyRot;
    this->yRotO = this->yRot;
    this->xRotO = this->xRot;
    ++this->tickCount;
    this->aiStep();
    float dx = this->x - this->xo;
    float dz = this->z - this->zo;
    float dst = std::sqrt((float)(dx*dx + dz*dz));
    float ybrot = this->yBodyRot;
    float animSpeed = 0.0f;
    this->oRun = this->run;
    float targetRun = 0.0f;
    if (dst > 0.05f) {
        targetRun = 1.0f;
        animSpeed = dst * 3.0f;
        ybrot = std::atan2(dz, dx) * 180.0f / M_PI - 90.0f;
    }

    if (!this->onGround) {
        targetRun = 0.0f;
    }

    this->run += (targetRun - this->run) * 0.3f;

    float bodyRot;
    for (bodyRot = ybrot - this->yBodyRot; bodyRot < -180.0f; bodyRot += 360.0f) {
        ;
    }

    while (bodyRot >= 180.0f) {
        bodyRot -= 360.0f;
    }

    this->yBodyRot += bodyRot * 0.1f;

    for (bodyRot = this->yRot - this->yBodyRot; bodyRot < -180.0f; bodyRot += 360.0f) {
        ;
    }
 
    while (bodyRot >= 180.0f) {
        bodyRot -= 360.0f;
    }

    bool movingBackwards = bodyRot < -90.0f || bodyRot > 90.0f;
    if (bodyRot < -75.0f) {
        bodyRot = -75.0f;
    }

    if (bodyRot >= 75.0f) {
        bodyRot = 75.0f;
    }

    this->yBodyRot = this->yRot - bodyRot;
    this->yBodyRot += bodyRot * 0.1f;
    if (movingBackwards) {
        animSpeed = -animSpeed;
    }

    while (this->yRot - this->yRotO < -180.0f) {
        this->yRotO -= 360.0f;
    }

    while (this->yRot - this->yRotO >= 180.0f) {
        this->yRotO += 360.0f;
    }

    while (this->yBodyRot - this->yBodyRotO < -180.0f) {
        this->yBodyRotO -= 360.0f;
    }

    while (this->yBodyRot - this->yBodyRotO >= 180.0f) {
        this->yBodyRotO += 360.0f;
    }

    while (this->xRot - this->xRotO < -180.0f) {
        this->xRotO -= 360.0f;
    }

    while (this->xRot - this->xRotO >= 180.0f) {
        this->xRotO += 360.0f;
    }

    this->animStep += animSpeed;
}

void Mob::aiStep() {
    if (this->ai != nullptr) {
        this->ai->tick(this->level, this);
    }
}

void Mob::bindTexture(Textures* textures) {
    glBindTexture(GL_TEXTURE_2D, textures->loadTexture(this->textureName, GL_NEAREST));
}

void Mob::render(float partialTicks, Textures* textures) {
    Entity::render(partialTicks, textures);
    if (this->modelName != "") {
        float attackProgress = (float)this->attackTime - partialTicks;
        if (attackProgress < 0.0f) {
            attackProgress = 0.0f;
        }

        while (this->yBodyRotO - this->yBodyRot < -180.0f) {
            this->yBodyRotO += 360.0f;
        }

        while (this->yBodyRotO - this->yBodyRot >= 180.0f) {
            this->yBodyRotO -= 360.0f;
        }

        while (this->xRotO - this->xRot < -180.0f) {
            this->xRotO += 360.0f;
        }

        while (this->xRotO - this->xRot >= 180.0f) {
            this->xRotO -= 360.0f;
        }

        while (this->yRotO - this->yRot < -180.0f) {
            this->yRotO += 360.0f;
        }

        while (this->yRotO - this->yRot >= 180.0f) {
            this->yRotO -= 360.0f;
        }

        float interpBodyYaw = this->yBodyRotO + (this->yBodyRot - this->yBodyRotO) * partialTicks;
        float interpRun = this->oRun + (this->run - this->oRun) * partialTicks;
        float interpHeadYaw = this->yRotO + (this->yRot - this->yRotO) * partialTicks;
        float interpHeadPitch = this->xRotO + (this->xRot - this->xRotO) * partialTicks;
        interpHeadYaw -= interpBodyYaw;
        glPushMatrix();
        float interpAnimStep = this->animStepO + (this->animStep - this->animStepO) * partialTicks;
        float brightness = this->getBrightness(partialTicks);
        glColor3f(brightness, brightness, brightness);
        float scale = 0.0625f;
        float bobOffset = (float)(-std::abs(std::cos((float)interpAnimStep * 0.6662f)) * 5.0f * interpRun * this->bobStrength - 23.0f);
        float interpX = this->xo + (this->x - this->xo) * partialTicks;
        float interpY = this->yo + (this->y - this->yo) * partialTicks - 1.62F + this->renderOffset;
        float interpZ = this->zo + (this->z - this->zo) * partialTicks;
        glTranslatef(interpX, interpY, interpZ);
        float hurtAngle = (float)this->hurtTime - partialTicks;
        if (hurtAngle > 0.0f || this->health <= 0) {
            if (hurtAngle < 0.0f) {
                hurtAngle = 0.0f;
            } else {
                hurtAngle = (float)std::sin((hurtAngle /= (float)this->hurtDuration) * hurtAngle * hurtAngle * hurtAngle * M_PI) * 14.0f;
            }

            float hurtData = 0.0f;
            if (this->health <= 0) {
                hurtData = ((float)this->deathTime + partialTicks) / 20.0f;
                if ((hurtAngle += hurtData*hurtData*800.0f) > 90.0f) {
                    hurtAngle = 90.0f;
                }
            }

            hurtData = this->hurtDir;
            glRotatef(180.0f - interpBodyYaw + this->rotOffs, 0.0f, 1.0f, 0.0f);
            glScalef(1.0f, 1.0f, 1.0f);
            glRotatef(-hurtData, 0.0f, 1.0f, 0.0f);
            glRotatef(-hurtAngle, 0.0f, 0.0f, 1.0f);
            glRotatef(hurtData, 0.0f, 1.0f, 0.0f);
            glRotatef(-(180.0f - interpBodyYaw + this->rotOffs), 0.0f, 1.0f, 0.0f);
        }

        glScalef(1.0f, -1.0f, 1.0f);
        glTranslatef(0.0f, bobOffset * scale, 0.0f);
        glRotatef(180.0f - interpBodyYaw + this->rotOffs, 0.0f, 1.0f, 0.0f);
        if (!this->allowAlpha) {
            glDisable(GL_ALPHA_TEST);
        } else {
            glDisable(GL_CULL_FACE);
        }

        glScalef(-1.0f, 1.0f, 1.0f);
        this->modelManager->getModel(this->modelName)->attackOffset = attackProgress / 5.0f;
        glEnable(GL_TEXTURE_2D);
        this->bindTexture(textures);
        this->renderModel(textures, interpAnimStep, partialTicks, interpRun, interpHeadYaw, interpHeadPitch, scale);
        if (this->invulnerableTime > this->invulnerableDuration - 10) {
            glColor4f(1.0f, 1.0f, 1.0f, 0.75f);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            this->bindTexture(textures);
            this->renderModel(textures, interpAnimStep, partialTicks, interpRun, interpHeadYaw, interpHeadPitch, scale);
            glDisable(GL_BLEND);
        }

        glDisable(GL_TEXTURE_2D);
        glEnable(GL_ALPHA_TEST);
        if (this->allowAlpha) {
            glEnable(GL_CULL_FACE);
        }

        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        glDisable(GL_TEXTURE_2D);
        glPopMatrix();
    }
}

void Mob::renderModel(Textures* textures, float time, float speed, float tick, float headYRot, float headXRot, float scale) {
    this->modelManager->getModel(this->modelName)->render(time, tick, (float)this->tickCount+speed, headYRot, headXRot, scale);
}

void Mob::travel(float xxa, float yya) {
    float oldY = 0.0f;
    if (this->isInWater()) {
        oldY = this->y;
        this->moveRelative(xxa, yya, 0.02f);
        this->move(this->xd, this->yd, this->zd);
        this->xd *= 0.8f;
        this->yd *= 0.8f;
        this->zd *= 0.8f;
        this->yd = this->yd - 0.02f;
        if (this->horizontalCollision && this->isFree(this->xd, this->yd + 0.6f - this->y + oldY, this->zd)) {
            this->yd = 0.3f;
        }
    } else if (this->isInLava()) {
        oldY = this->y;
        this->moveRelative(xxa, yya, 0.02f);
        this->move(this->xd, this->yd, this->zd);
        this->xd *= 0.5f;
        this->yd *= 0.5f;
        this->zd *= 0.5f;
        this->yd = this->yd - 0.02f;
        if (this->horizontalCollision && this->isFree(this->xd, this->yd + 0.6f - this->y + oldY, this->zd)) {
            this->yd = 0.3f;
        }
    } else {
        this->moveRelative(xxa, yya, this->onGround ? 0.1f : 0.02f);
        this->move(this->xd, this->yd, this->zd);
        this->xd *= 0.91f;
        this->yd *= 0.98f;
        this->zd *= 0.91f;
        this->yd = (float)((float)this->yd - 0.08f);
        if (this->onGround) {
            this->xd *= 0.6f;
            this->zd *= 0.6f;
        }
    }
}

void Mob::causeFallDamage(float fall) {
    int dmg = std::ceil(fall - 3.0f);
    if (dmg > 0) {
        this->hurt((Entity*)nullptr, dmg);
    }
}

void Mob::hurt(Entity* e, int dmg) {
    if (this->health > 0) {
        if ((float)this->invulnerableTime > (float)this->invulnerableDuration / 2.0f) {
            if (this->lastHealth - dmg >= this->health) {
                return;
            }

            this->health = this->lastHealth - dmg;
        } else {
            this->lastHealth = this->health;
            this->invulnerableTime = this->invulnerableDuration;
            this->health -= dmg;
            this->hurtTime = this->hurtDuration = 10;
        }

        this->hurtDir = 0.0f;
        if (e != nullptr) {
            float dx = e->x - this->x;
            float dz = e->z - this->z;
            this->hurtDir = (float)(std::atan2((float)dz, (float)dx) * 180.0f / M_PI) - this->yRot;
            this->knockback(e, dmg, dx, dz);
        } else {
            this->hurtDir = (float)((int)(Random::random() * 2.0f) * 180); 
        }

        if (this->health <= 0) {
            this->die(e);
        }
    }
}

void Mob::knockback(Entity* e, int dmg, float dx, float dz) {
    float dst = (float)std::sqrt(dx*dx + dz*dz);
    float strength = 0.4f;
    this->xd /= 2.0f;
    this->yd /= 2.0f;
    this->zd /= 2.0f;
    this->xd -= dx / dst*strength;
    this->yd += 0.4f;
    this->zd -= dz / dst*strength;
    if (this->yd > 0.4f) {
        this->yd = 0.4f;
    }
}

void Mob::die(Entity* e) {
    this->dead = true;
}

bool Mob::isShootable() {
    return true;
}

void Mob::heal(int hp) {
    if (this->health > 0) {
        this->health += hp;
        if (this->health > 20) {
            this->health = 20;
        }

        this->invulnerableTime = this->invulnerableDuration / 2;
    }
}