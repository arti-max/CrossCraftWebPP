#pragma once
#include "model/ModelPart.hpp"

class SignModel {
public:
    ModelPart* p1 = new ModelPart(0, 0);
    ModelPart* p2;

    SignModel() {
        this->p1->addBox(-12.0f, -14.0f, -1.0f, 24, 12, 2, 0.0f);
        this->p2 = new ModelPart(0, 14);
        this->p2->addBox(-1.0f, -2.0f, -1.0f, 2, 14, 2, 0.0f);
    }

    void render(float scale) {
        this->p1->render(scale);
        this->p2->render(scale);
    }

    ~SignModel() {
        delete p1;
        delete p2;
    }
};