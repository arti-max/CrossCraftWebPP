#pragma once
#include <string>
#include "model/Model.hpp"
#include "model/HumanModel.hpp"
#include "model/ZombieModel.hpp"
#include "model/SkeletonModel.hpp"

class ModelManager {
private:
    HumanModel* human = new HumanModel(0.0f);
    HumanModel* armor = new HumanModel(1.0f);
    ZombieModel* zombie = new ZombieModel();
    SkeletonModel* skeleton = new SkeletonModel();
public:

    Model* getModel(std::string name) {
        if (name == "human") {
            return (Model*)this->human;
        }
        if (name == "human.armor") {
            return (Model*)this->armor;
        }
        if (name == "zombie") {
            return (Model*)this->zombie;
        }
        if (name == "skeleton") {
            return (Model*)this->skeleton;
        }

        return (Model*)this->human;
    }
};