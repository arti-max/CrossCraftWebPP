#pragma once
#include "model/HumanModel.hpp"

class ZombieModel : public HumanModel {
public:
    void setRotation(float time, float speed, float tick, float headYRot, float headXRot, float scale) override {
        HumanModel::setRotation(time, speed, tick, headYRot, headXRot, scale);
        float attackSwing = std::sin(this->attackOffset * M_PI);
        float attackRecover = std::sin((1.0f - (1.0f - this->attackOffset) * (1.0f - this->attackOffset)) * M_PI);
        this->arm0->roll = 0.0f;
        this->arm1->roll = 0.0f;
        this->arm0->yaw = -(0.1f - attackSwing * 0.6f);
        this->arm1->yaw = 0.1f - attackSwing * 0.6f;
        this->arm0->pitch = -1.5707964f;
        this->arm1->pitch = -1.5707964f;
        this->arm0->pitch -= attackSwing * 1.2f - attackRecover * 0.4f;
        this->arm1->pitch -= attackSwing * 1.2f - attackRecover * 0.4f;
        this->arm0->roll += std::cos(tick * 0.09f) * 0.05f + 0.05f;
        this->arm1->roll -= std::cos(tick * 0.09f) * 0.05f + 0.05f;
        this->arm0->pitch += std::sin(tick * 0.067f) * 0.05f;
        this->arm0->pitch -= std::sin(tick * 0.067f) * 0.05f;
    }
};
