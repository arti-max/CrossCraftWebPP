#pragma once
#include <string>
#include "model/Model.hpp"
#include "model/HumanModel.hpp"

class ModelManager {
private:
    HumanModel* human = new HumanModel(0.0f);
    HumanModel* armor = new HumanModel(1.0f);
public:

    Model* getModel(std::string name) {
        if (name == "human") {
            return this->human;
        }
        if (name == "human.armor") {
            return this->armor;
        }
    }
};