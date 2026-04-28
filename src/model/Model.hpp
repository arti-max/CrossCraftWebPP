#pragma once

class Model {
public:
    float attackOffset;

    virtual void render(float time, float speed, float tick, float headYRot, float headXRot, float scale) = 0;
};