#pragma once
#include "model/Model.hpp"
#include "model/ModelPart.hpp"

class PigModel : public Model {
public:
    ModelPart* head = new ModelPart(0, 0);
    ModelPart* body;
    ModelPart* leg1;
    ModelPart* leg2;
    ModelPart* leg3;
    ModelPart* leg4;

    PigModel(int legHeight, float scale) {
        this->head->addBox(-4.0f, -4.0f, -8.0f, 8, 8, 8, 0.0f);
        this->head->setPosition(0.0f, (float)(18 - legHeight), -6.0f);
        this->body = new ModelPart(28, 8);
        this->body->addBox(-5.0f, -10.0f, -7.0f, 10, 16, 8, 0.0f);
        this->body->setPosition(0.0f, (float)(17 - legHeight), 2.0f);
        this->leg1 = new ModelPart(0, 16);
        this->leg1->addBox(-2.0f, 0.0f, -2.0f, 4, legHeight, 4, 0.0f);
        this->leg1->setPosition(-3.0f, (float)(24 - legHeight), 7.0f);
        this->leg2 = new ModelPart(0, 16);
        this->leg2->addBox(-2.0f, 0.0f, -2.0f, 4, legHeight, 4, 0.0f);
        this->leg2->setPosition(3.0f, (float)(24 - legHeight), 7.0f);
        this->leg3 = new ModelPart(0, 16);
        this->leg3->addBox(-2.0f, 0.0f, -2.0f, 4, legHeight, 4, 0.0f);
        this->leg3->setPosition(-3.0f, (float)(24 - legHeight), -5.0f);
        this->leg4 = new ModelPart(0, 16);
        this->leg4->addBox(-2.0f, 0.0f, -2.0f, 4, legHeight, 4, 0.0f);
        this->leg4->setPosition(3.0f, (float)(24 - legHeight), -5.0f);
    }
    ~PigModel() {
        delete head;
        delete body;
        delete leg1;
        delete leg2;
        delete leg3;
        delete leg4;
    }

    void render(float time, float speed, float tick, float headYRot, float headXRot, float scale) override {
        this->head->yaw = headYRot / 57.295776f;
        this->head->pitch = headXRot / 57.295776f;
        this->body->pitch = 1.5707964f;
        this->leg1->pitch = std::cos(time * 0.6662f) * 1.4f * speed;
        this->leg2->pitch = std::cos(time * 0.6662f + M_PI) * 1.4f * speed;
        this->leg3->pitch = std::cos(time * 0.6662f + M_PI) * 1.4f * speed;
        this->leg4->pitch = std::cos(time * 0.6662f) * 1.4f * speed;
        this->head->render(scale);
        this->body->render(scale);
        this->leg1->render(scale);
        this->leg2->render(scale);
        this->leg3->render(scale);
        this->leg4->render(scale);
    }
};