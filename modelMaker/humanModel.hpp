#pragma once
#include "model/ModelPart.hpp"
#include <cmath>

class humanModel {
public:
    ModelPart* head;
    ModelPart* body;
    ModelPart* arm0;
    ModelPart* arm1;
    ModelPart* leg0;
    ModelPart* leg1;

    humanModel();
    ~humanModel();
    
    void render(float time, float speed, float tick, float headYRot, float headXRot, float scale);
};

// Implementation
#include <iostream>

humanModel::humanModel() {
    head = new ModelPart(0, 0);
    head->addBox(-4.0f, -8.0f, -4.0f, 8, 8, 8);
    head->setPosition(0.0f, 0.0f, 0.0f);

    body = new ModelPart(16, 16);
    body->addBox(-4.0f, 0.0f, -2.0f, 8, 12, 4);
    body->setPosition(0.0f, 0.0f, 0.0f);

    arm0 = new ModelPart(40, 16);
    arm0->addBox(-3.0f, -2.0f, -2.0f, 4, 12, 4);
    arm0->setPosition(-5.0f, 2.0f, 0.0f);

    arm1 = new ModelPart(40, 16);
    arm1->addBox(-1.0f, -2.0f, -2.0f, 4, 12, 4);
    arm1->setPosition(5.0f, 2.0f, 0.0f);

    leg0 = new ModelPart(0, 16);
    leg0->addBox(-2.0f, 0.0f, -2.0f, 4, 12, 4);
    leg0->setPosition(-2.0f, 12.0f, 0.0f);

    leg1 = new ModelPart(0, 16);
    leg1->addBox(-2.0f, 0.0f, -2.0f, 4, 12, 4);
    leg1->setPosition(2.0f, 12.0f, 0.0f);

}

humanModel::~humanModel() {
    delete head;
    delete body;
    delete arm0;
    delete arm1;
    delete leg0;
    delete leg1;
}

void humanModel::render(float time, float speed, float tick, float headYRot, float headXRot, float scale) {
    // TODO: Add animation logic
    this->head->render(scale);
    this->body->render(scale);
    this->arm0->render(scale);
    this->arm1->render(scale);
    this->leg0->render(scale);
    this->leg1->render(scale);
}