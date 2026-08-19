#pragma once
#include "mob/ai/BasicAI.hpp"
#include "player/Player.hpp"

// class Player {
// public:
//     bool input_jumping = false;
//     float input_xxa = 0.0f;
//     float input_yya = 0.0f;
// };

class PlayerAI : public BasicAI {
private:
    Player* player = nullptr;
public:
    PlayerAI(Player* player) {
        this->player = player;
    }

    void update() override {
        this->jumping = this->player->input_jumping;

        this->xxa = this->player->input_xxa;
        this->yya = this->player->input_yya;
    }

    void beforeRemove() override {
        this->player->removeExternally = true;
    }
};