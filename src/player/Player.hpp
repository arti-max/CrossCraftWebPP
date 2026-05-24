#pragma once
#include "Entity.hpp"
#include "mob/Mob.hpp"
#include "player/Inventory.hpp"
#include "Settings.hpp"
#include "EntityType.hpp"

class Textures;
class CrossCraft;

class Player : public Mob {
public:
    static const int MAX_HEALTH = 20;
    static const int KEY_UP = 0;
    static const int KEY_DOWN = 1;
    static const int KEY_LEFT = 2;
    static const int KEY_RIGHT = 3;
    static const int KEY_JUMP = 4;

    float oBob = 0.0f;
    float bob = 0.0f;
    float input_xxa = 0.0f;
    float input_yya = 0.0f;
    bool input_jumping = false;
    
    int score = 0;

    int newTextureId = -1;

private:
    bool keys[10] = {false};
    Settings* settings;

protected:
    bool isPlayer() override;

public:
    Player(Level* level, Settings* settings);
    
    Inventory* inventory = new Inventory(this);

    void aiStep() override;
    void die(Entity* e) override;
    void hurt(Entity* e, int dmg) override;
    void setKey();
    void releaseAllKeys();
    void resetPos() override;
    void bindTexture(CrossCraft* cc);
    HumanModel* getModel();
    void awardKillScore(Entity* e, int score) override;
    int getScore();

    EntityType getEntityType() const override { return EntityType::Player; }
};
