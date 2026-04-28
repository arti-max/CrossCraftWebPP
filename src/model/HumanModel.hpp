#pragma once
#include "model/Model.hpp"
#include "model/ModelPart.hpp"

class HumanModel : public Model {
public:
    ModelPart* head;
    ModelPart* headwear;
    ModelPart* body;
    ModelPart* arm0;
    ModelPart* arm1;
    ModelPart* leg0;
    ModelPart* leg1;

    HumanModel() : HumanModel(0.0f) {}
    ~HumanModel() {
        delete head;
        delete headwear;
        delete body;
        delete arm0;
        delete arm1;
        delete leg0;
        delete leg1;
    }

    HumanModel(float expand) {
        head = new ModelPart(0, 0);
        head->addBox(-4.0f, -8.0f, -4.0f, 8, 8, 8, expand);
        
        headwear = new ModelPart(32, 0);
        headwear->addBox(-4.0f, -8.0f, -4.0f, 8, 8, 8, expand + 0.5f);

        body = new ModelPart(16, 16);
        body->addBox(-4.0f, 0.0f, -2.0f, 8, 12, 4, expand);
        
        arm0 = new ModelPart(40, 16);
        arm0->addBox(-3.0f, -2.0f, -2.0f, 4, 12, 4, expand);
        arm0->setPosition(-5.0f, 2.0f, 0.0f);
        
        arm1 = new ModelPart(40, 16);
        arm1->mirror = true;
        arm1->addBox(-1.0f, -2.0f, -2.0f, 4, 12, 4, expand);
        arm1->setPosition(5.0f, 2.0f, 0.0f);
        
        leg0 = new ModelPart(0, 16);
        leg0->addBox(-2.0f, 0.0f, -2.0f, 4, 12, 4, expand);
        leg0->setPosition(-2.0f, 12.0f, 0.0f);

        leg1 = new ModelPart(0, 16);
        leg1->mirror = true;
        leg1->addBox(-2.0f, 0.0f, -2.0f, 4, 12, 4, expand);
        leg1->setPosition(2.0f, 12.0f, 0.0f);
    }
    
    void render(float time, float speed, float tick, float headYRot, float headXRot, float scale) override {
        this->setRotation(time, speed, tick, headYRot, headXRot, scale);
        this->head->render(scale);
        this->body->render(scale);
        this->arm0->render(scale);
        this->arm1->render(scale);
        this->leg0->render(scale);
        this->leg1->render(scale);
    }

    virtual void setRotation(float time, float speed, float tick, float headYRot, float headXRot, float scale) {
        this->head->yaw = headYRot / 57.29578f;
        this->head->pitch = headXRot / 57.29578f;
        
        this->arm0->pitch = std::cos(time * 0.6662f + M_PI) * 2.0f * speed;
        this->arm0->roll = (std::cos(time * 0.2312f) + 1.0f) * speed;
        
        this->arm1->pitch = std::cos(time * 0.6662f) * 2.0f * speed;
        this->arm1->roll = (std::cos(time * 0.2812f) - 1.0f) * speed;

        this->leg0->pitch = std::cos(time * 0.6662f) * 1.4f * speed;
        this->leg1->pitch = std::cos(time * 0.6662f + M_PI) * 1.4f * speed;
        
        this->arm0->roll += std::cos(tick * 0.09f) * 0.05f + 0.05f;
        this->arm1->roll -= std::cos(tick * 0.09f) * 0.05f + 0.05f;
        this->arm0->pitch += std::sin(tick * 0.067f) * 0.05f;
        this->arm1->pitch -= std::sin(tick * 0.067f) * 0.05f;
        
    }
};