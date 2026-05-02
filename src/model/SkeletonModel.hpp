#pragma once
#include "model/ZombieModel.hpp"

class SkeletonModel : public ZombieModel {
public:
    SkeletonModel() {
        this->arm0 = new ModelPart(40, 16);
        this->arm0->addBox(-1.0f, -2.0f, -1.0f, 2, 12, 2, 0.0f);
        this->arm0->setPosition(-5.0f, 2.0f, 0.0f);
        this->arm1 = new ModelPart(40, 16);
        this->arm1->mirror = true;
        this->arm1->addBox(-1.0f, -2.0f, -1.0f, 2, 12, 2, 0.0f);
        this->arm1->setPosition(5.0f, 2.0f, 0.0f);
        this->leg0 = new ModelPart(0, 16);
        this->leg0->addBox(-1.0f, 0.0f, -1.0f, 2, 12, 2, 0.0f);
        this->leg0->setPosition(-2.0f, 12.0f, 0.0f);
        this->leg1 = new ModelPart(0, 16);
        this->leg1->mirror = true;
        this->leg1->addBox(-1.0f, 0.0f, -1.0f, 2, 12, 2, 0.0f);
        this->leg1->setPosition(2.0f, 12.0f, 0.0f);
    }
};