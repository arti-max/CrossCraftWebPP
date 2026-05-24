#pragma once
#include <string>
#include "model/Model.hpp"
#include "model/HumanModel.hpp"
#include "model/ZombieModel.hpp"
#include "model/SkeletonModel.hpp"
#include "model/PigModel.hpp"
#include "model/CreeperModel.hpp"

class ModelManager {
private:
    HumanModel* human = new HumanModel(0.0f);
    HumanModel* armor = new HumanModel(1.0f);
    ZombieModel* zombie = new ZombieModel();
    SkeletonModel* skeleton = new SkeletonModel();
    CreeperModel* creeper = new CreeperModel();
    PigModel* pig = new PigModel(6, 0.0f);
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
        if (name == "pig") {
            return (Model*)this->pig;
        }
        if (name == "creeper") {
            return (Model*)this->creeper;
        }

        return (Model*)this->human;
    }
};