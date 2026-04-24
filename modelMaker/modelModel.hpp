#pragma once
#include "model/ModelPart.hpp"
#include <cmath>

class modelModel {
public:
    ModelPart* head;

    modelModel();
    ~modelModel();
    
    void render(float time, float speed, float tick, float headYRot, float headXRot, float scale);
};

// Implementation
#include <iostream>

modelModel::modelModel() {
    head = new ModelPart(0, 0);
    head->addBox(-4.0f, -8.0f, -4.0f, 8, 8, 8);
    head->setPosition(0.0f, 0.0f, 0.0f);

}

modelModel::~modelModel() {
    delete head;
}

void modelModel::render(float time, float speed, float tick, float headYRot, float headXRot, float scale) {
    // TODO: Add animation logic
    this->head->render(scale);
}