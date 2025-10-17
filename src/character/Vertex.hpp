#pragma once
#include "character/Vec3.hpp"

class Vertex {
public:
    Vec3* pos;
    float u;
    float v;
    Vertex(float x, float y, float z, float u, float v) : Vertex(new Vec3(x, y, z), u, v) {}
    Vertex(Vertex* vertex, float u, float v) {
        this->pos = vertex->pos;
        this->u = u;
        this->v = v;
    }
    Vertex(Vec3* pos, float u, float v) {
        this->pos = pos;
        this->u = u;
        this->v = v;
    }
    Vertex* remap(float u, float v) {
        return new Vertex(this, u, v);
    }
};