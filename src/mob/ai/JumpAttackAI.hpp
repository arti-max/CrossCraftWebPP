#pragma once
#include "mob/ai/BasicAttackAI.hpp"


class JumpAttackAI : public BasicAttackAI {
public:
    JumpAttackAI();
    void jumpFromGround() override;
};