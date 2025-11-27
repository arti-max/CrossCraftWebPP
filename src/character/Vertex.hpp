#pragma once
#include "character/Vec3.hpp"

class Vertex {
public:
    Vec3 pos;
    float u, v;
    
    Vertex(float x, float y, float z, float u, float v) : pos(x, y, z), u(u), v(v) {}
    
    Vertex(const Vec3& _pos, float u, float v) : pos(_pos), u(u), v(v) {}
    
    Vertex remap(float u, float v) const {
        return Vertex(this->pos, u, v);
    }
};
