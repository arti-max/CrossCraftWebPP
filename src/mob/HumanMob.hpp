#pragma once
#include "mob/Mob.hpp"
#include "model/Model.hpp"
#include "model/HumanModel.hpp"


class Level;
class Textures;

class HumanMob : public Mob {
private:
    bool armor = Random::random() < 0.2f;
    bool helmet = Random::random() < 0.2f;
public:
    HumanMob(Level* level, float x, float y, float z);
    void renderModel(Textures* textures, float time, float speed, float tick, float headYRot, float headXRot, float scale) override;
    EntityType getEntityType() const override { return EntityType::HumanMob; }
};