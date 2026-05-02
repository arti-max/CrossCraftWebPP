#pragma once
#include "model/Vec3D.hpp"
#include <cmath>

class NewVertex {
public:
    Vec3D vector;
    float u = 0.0f;
    float v = 0.0f;

    NewVertex(float x, float y, float z, float u, float v) {
        this->vector = Vec3D(x, y, z);
        this->u = u;
        this->v = v;
    }

    NewVertex create(float u, float v) {
        return NewVertex(this, u, v);
    }

    NewVertex(const Vec3D& vec, float u, float v) {
        this->vector = vec;
        this->u = u;
        this->v = v;
    }

    NewVertex() : vector(0,0,0), u(0), v(0) {}

private:
    NewVertex(NewVertex* vrtx, float u, float v) {
        this->vector = vrtx->vector;
        this->u = u;
        this->v = v;
    }
};