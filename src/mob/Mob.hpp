#pragma once
#include <string>
#include "Entity.hpp"
#include "mob/ai/AI.hpp"
#include "model/ModelManager.hpp"
#include "util/Random.hpp"

class Textures;
class Level;

class Mob : public Entity {
public:
    static const int ATTACK_DURATION = 5;
    static const int TOTAL_AIR_SUPPLY = 300;
    static ModelManager* modelManager;
    int invulnerableDuration = 20;
    float rot;
    float timeOffs;
    float speed;
    float rotA = (float)(Random::random() + 1.0f) * 0.01f;
    float yBodyRot = 0.0f;
    float yBodyRotO = 0.0f;
    float oRun;
    float run;
    float animStep;
    float animStepO;
    int tickCount = 0;
    bool hasHair = true;
    std::string textureName = "char";
    bool allowAlpha = true;
    float rotOffs = 0.0f;
    std::string modelName = "";
    float bobStrength = 1.0f;
    int deathScore = 0.0f;
    float renderOffset = 0.0f;
    int health = 20;
    int lastHealth;
    int invulnerableTime = 0;
    int airSupply = 300;
    int hurtTime = 0;
    int hurtDuration = 10;
    float hurtDir = 0.0f;
    int deathTime = 0;
    int attackTime = 0;
    float oTilt;
    float tilt;
    bool dead = false;
    AI* ai;

    Mob(Level* level);


    bool isPickable() override;
    void travel(float xxa, float yya);
    bool isPushable() override;
    void tick() override;
    virtual void aiStep();
    virtual void bindTexture(Textures* texture);
    void render(float partialTicks, Textures* textures) override;
    virtual void renderModel(Textures* textures, float time, float speed, float tick, float headYRot, float headXRot, float scale);
    virtual void heal(int hp);
    virtual void hurt(Entity* e, int dmg);
    virtual void knockback(Entity* e, int dmg, float dx, float dz);
    void die(Entity* e);
    bool isShootable() override;
protected:
    void causeFallDamage(float fall) override;
};