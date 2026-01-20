#pragma once
#include "Entity.hpp"
#include "player/Inventory.hpp"
#include "Settings.hpp"

class Player : public Entity {
public:
    static const int KEY_UP = 0;
    static const int KEY_DOWN = 1;
    static const int KEY_LEFT = 2;
    static const int KEY_RIGHT = 3;
    static const int KEY_JUMP = 4;

private:
    bool keys[10] = {false};
    Settings* settings;

public:
    Player(Level* level, Settings* settings);
    
    Inventory* inventory = new Inventory(this);

    void setKey();
    void releaseAllKeys();
    void tick() override;
};
