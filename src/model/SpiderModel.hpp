#pragma once
#include "model/Model.hpp"
#include "model/ModelPart.hpp"

class SpiderModel : public Model {
private:
    ModelPart* head = new ModelPart(32, 4);
    ModelPart* neck = new ModelPart(0, 0);
    ModelPart* body = new ModelPart(0, 12);
    ModelPart* leg1 = new ModelPart(18, 0);
    ModelPart* leg2 = new ModelPart(18, 0);
    ModelPart* leg3 = new ModelPart(18, 0);
    ModelPart* leg4 = new ModelPart(18, 0);
    ModelPart* leg5 = new ModelPart(18, 0);
    ModelPart* leg6 = new ModelPart(18, 0);
    ModelPart* leg7 = new ModelPart(18, 0);
    ModelPart* leg8 = new ModelPart(18, 0);
public:
    SpiderModel() {
        this->head->addBox(-4.0f, -4.0f, -8.0f, 8, 8, 8);
        this->head->setPosition(0.0f, 0.0f, -3.0f);
        this->neck->addBox(-3.0f, -3.0f, -3.0f, 6, 6, 6);
        this->body->addBox(-5.0f, -4.0f, -6.0f, 10, 8, 12);
        this->body->setPosition(0.0f, 0.0f, 9.0f);
        // legs
        this->leg1->addBox(-15.0f, -1.0f, -1.0f, 16, 2, 2);
        this->leg1->setPosition(-4.0f, 0.0f, 2.0f);
        this->leg2->addBox(-1.0f, -1.0f, -1.0f, 16, 2, 2);
        this->leg2->setPosition(4.0f, 0.0f, 2.0f);
        this->leg3->addBox(-15.0f, -1.0f, -1.0f, 16, 2, 2);
        this->leg3->setPosition(-4.0f, 0.0f, 1.0f);
        this->leg4->addBox(-1.0f, -1.0f, -1.0f, 16, 2, 2);
        this->leg4->setPosition(4.0f, 0.0f, 1.0f);
        this->leg5->addBox(-15.0f, -1.0f, -1.0f, 16, 2, 2);
        this->leg5->setPosition(-4.0f, 0.0f, 0.0f);
        this->leg6->addBox(-1.0f, -1.0f, -1.0f, 16, 2, 2);
        this->leg6->setPosition(4.0f, 0.0f, 0.0f);
        this->leg7->addBox(-15.0f, -1.0f, -1.0f, 16, 2, 2);
        this->leg7->setPosition(-4.0f, 0.0f, -1.0f);
        this->leg8->addBox(-1.0f, -1.0f, -1.0f, 16, 2, 2);
        this->leg8->setPosition(4.0f, 0.0f, -1.0f);
    }
    ~SpiderModel() {
        delete head;
        delete neck;
        delete body;
        delete leg1;
        delete leg2;
        delete leg3;
        delete leg4;
        delete leg5;
        delete leg6;
        delete leg7;
        delete leg8;
    }

    void render(float time, float speed, float tick, float headYRot, float headXRot, float scale) override {
        this->head->yaw = headYRot / (180.0f / M_PI);
        this->head->pitch = headXRot / (180.0f / M_PI);
        float rot1 = (M_PI / 4.0f); // 45
        this->leg1->roll = -rot1;
        this->leg2->roll = rot1;
        this->leg3->roll = -rot1 * 0.74f;
        this->leg4->roll = rot1 * 0.74f;
        this->leg5->roll = -rot1 * 0.74f;
        this->leg6->roll = rot1 * 0.74f;
        this->leg7->roll = -rot1;
        this->leg8->roll = rot1;
        float rot2 = (M_PI / 8.0f); // 25
        this->leg1->yaw = rot2 * 2.0f;
        this->leg2->yaw = -rot2 * 2.0f;
        this->leg3->yaw = rot2;
        this->leg4->yaw = -rot2;
        this->leg5->yaw = -rot2;
        this->leg6->yaw = rot2;
        this->leg7->yaw = -rot2 * 2.0f;
        this->leg8->yaw = rot2 * 2.0f;
        float yawOffset1 = -(std::cos(time * 0.6662f * 2.0f) * 0.4f) * speed;
        float yawOffset2 = -(std::cos(time * 0.6662f * 2.0f + M_PI) * 0.4f) * speed;
        float yawOffset3 = -(std::cos(time * 0.6662f * 2.0f + (M_PI / 2.0f)) * 0.4f) * speed;
        float yawOffset4 = -(std::cos(time * 0.6662f * 2.0f + (M_PI * 1.5f)) * 0.4f) * speed;

        float rollOffset1 = std::abs(std::sin(time* 0.6662f) * 0.4f) * speed;
        float rollOffset2 = std::abs(std::sin(time* 0.6662f + M_PI) * 0.4f) * speed;
        float rollOffset3 = std::abs(std::sin(time* 0.6662f + (M_PI / 2.0f)) * 0.4f) * speed;
        float rollOffset4 = std::abs(std::sin(time* 0.6662f + (M_PI * 1.5f)) * 0.4f) * speed;

        this->leg1->yaw += yawOffset1;
        this->leg2->yaw -= yawOffset1;
        this->leg3->yaw += yawOffset2;
        this->leg4->yaw -= yawOffset2;
        this->leg5->yaw += yawOffset3;
        this->leg6->yaw -= yawOffset3;
        this->leg7->yaw += yawOffset4;
        this->leg8->yaw -= yawOffset4;

        this->leg1->roll += rollOffset1;
        this->leg2->roll -= rollOffset1;
        this->leg3->roll += rollOffset2;
        this->leg4->roll -= rollOffset2;
        this->leg5->roll += rollOffset3;
        this->leg6->roll -= rollOffset3;
        this->leg7->roll += rollOffset4;
        this->leg8->roll -= rollOffset4;

        this->head->render(scale);
        this->neck->render(scale);
        this->body->render(scale);
        this->leg1->render(scale);
        this->leg2->render(scale);
        this->leg3->render(scale);
        this->leg4->render(scale);
        this->leg5->render(scale);
        this->leg6->render(scale);
        this->leg7->render(scale);
        this->leg8->render(scale);
    }
};