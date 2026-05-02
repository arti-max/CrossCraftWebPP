#include "character/ZombieModel.hpp"
#include <cmath>
#include <iostream>

HZombieModel::HZombieModel() {
    
    head = new Cube(0, 0);
    head->addBox(-4.0f, -8.0f, -4.0f, 8, 8, 8);
    
    body = new Cube(16, 16);
    body->addBox(-4.0f, 0.0f, -2.0f, 8, 12, 4);
    
    arm0 = new Cube(40, 16);
    arm0->addBox(-3.0f, -2.0f, -2.0f, 4, 12, 4);
    arm0->setPos(-5.0f, 2.0f, 0.0f);
    
    arm1 = new Cube(40, 16);
    arm1->addBox(-1.0f, -2.0f, -2.0f, 4, 12, 4);
    arm1->setPos(5.0f, 2.0f, 0.0f);
    
    leg0 = new Cube(0, 16);
    leg0->addBox(-2.0f, 0.0f, -2.0f, 4, 12, 4);
    leg0->setPos(-2.0f, 12.0f, 0.0f);

    leg1 = new Cube(0, 16);
    leg1->addBox(-2.0f, 0.0f, -2.0f, 4, 12, 4);
    leg1->setPos(2.0f, 12.0f, 0.0f);
}

HZombieModel::~HZombieModel() {
    delete head;
    delete body;
    delete arm0;
    delete arm1;
    delete leg0;
    delete leg1;
}

void HZombieModel::render(float time, float speed, float tick, float headYRot, float headXRot, float scale) {
    
    this->head->yRot = headYRot / 57.29578f;
    this->head->xRot = headXRot / 57.29578f;
    
    this->arm0->xRot = std::cos(time * 0.6662f + M_PI) * 2.0f * speed;
    this->arm0->zRot = (std::cos(time * 0.2312f) + 1.0f) * speed;
    
    this->arm1->xRot = std::cos(time * 0.6662f) * 2.0f * speed;
    this->arm1->zRot = (std::cos(time * 0.2812f) - 1.0f) * speed;

    this->leg0->xRot = std::cos(time * 0.6662f) * 1.4f * speed;
    this->leg1->xRot = std::cos(time * 0.6662f + M_PI) * 1.4f * speed;
    
    this->arm0->zRot += std::cos(tick * 0.09f) * 0.05f + 0.05f;
    this->arm1->zRot -= std::cos(tick * 0.09f) * 0.05f + 0.05f;
    this->arm0->xRot += std::sin(tick * 0.067f) * 0.05f;
    this->arm1->xRot -= std::sin(tick * 0.067f) * 0.05f;
    
    this->head->render(scale);
    this->body->render(scale);
    this->arm0->render(scale);
    this->arm1->render(scale);
    this->leg0->render(scale);
    this->leg1->render(scale);
}
