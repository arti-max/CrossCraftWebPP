#include "mob/ai/JumpAttackAI.hpp"
#include "mob/ai/BasicAttackAI.hpp"
#include "mob/Mob.hpp"

JumpAttackAI::JumpAttackAI() {
    this->runSpeed *= 0.8f;
}

void JumpAttackAI::jumpFromGround() {
    if (this->attackTarget == nullptr) {
        BasicAttackAI::jumpFromGround();
    } else {
        this->mob->xd = 0.0f;
        this->mob->zd = 0.0f;
        this->mob->moveRelative(0.0f, 1.0f, 0.6f);
        this->mob->yd = 0.5f;
    }
}