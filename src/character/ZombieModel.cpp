#include "character/ZombieModel.hpp"
#include <cmath>
#include <iostream>

ZombieModel::ZombieModel() {
    
    head = new Cube(0, 0);
    head->addBox(-4.0f, -8.0f, -4.0f, 8, 8, 8);
    
    body = new Cube(16, 16);
    body->addBox(-4.0f, 0.0f, -2.0f, 8, 12, 4);
    
    arm0 = new Cube(40, 16);
    arm0->addBox(-3.0f, -2.0f, -2.0f, 4, 12, 4);
    arm0->setPos(-5.0f, 2.0f, 0.0f);
    
    arm1 = new Cube(40, 16);
    arm1->addBox(-1.0f, 0.0f, -2.0f, 4, 12, 4);
    arm1->setPos(5.0f, 2.0f, 0.0f);
    
    leg0 = new Cube(0, 16);
    leg0->addBox(-2.0f, 0.0f, -2.0f, 4, 12, 4);
    leg0->setPos(-2.0f, 12.0f, 0.0f);

    leg1 = new Cube(0, 16);
    leg1->addBox(-2.0f, 0.0f, -2.0f, 4, 12, 4);
    leg1->setPos(2.0f, 12.0f, 0.0f);
}

ZombieModel::~ZombieModel() {
    delete head;
    delete body;
    delete arm0;
    delete arm1;
    delete leg0;
    delete leg1;
}

void ZombieModel::render(float time) {
    
    head->yRot = std::sin(time * 0.83f);
    head->xRot = std::sin(time) * 0.8f;
    
    arm0->xRot = std::sin(time * 0.6662f + M_PI) * 2.0f;
    arm0->zRot = (std::sin(time * 0.2312f) + 1.0f);
    
    arm1->xRot = std::sin(time * 0.6662f) * 2.0f;
    arm1->zRot = (std::sin(time * 0.2312f) - 1.0f);
    
    leg0->xRot = std::sin(time * 0.6662f) * 1.4f;
    leg1->xRot = std::sin(time * 0.6662f + M_PI) * 1.4f;
    
    head->render();
    body->render();
    arm0->render();
    arm1->render();
    leg0->render();
    leg1->render();
}
