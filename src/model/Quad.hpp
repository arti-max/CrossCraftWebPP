#pragma once
#include "model/Vertex.hpp"
#include <vector>


class Quad {
public:
    std::vector<NewVertex> vertices;

    Quad() {}

    Quad(const std::vector<NewVertex>& verts, int u0, int v0, int u1, int v1) {
        this->vertices = verts;

        float uu = 0.0015625f;
        float vv = 0.003125f;

        this->vertices[0] = this->vertices[0].create((float)u1 / 64.0f - uu, (float)v0 / 32.0f + vv);
        this->vertices[1] = this->vertices[1].create((float)u0 / 64.0f + uu, (float)v0 / 32.0f + vv);
        this->vertices[2] = this->vertices[2].create((float)u0 / 64.0f + uu, (float)v1 / 32.0f - vv);
        this->vertices[3] = this->vertices[3].create((float)u1 / 64.0f - uu, (float)v1 / 32.0f - vv);
    }

    Quad(const std::vector<NewVertex>& verts, float u0, float v0, float u1, float v1) {
        this->vertices = verts;

        this->vertices[0] = this->vertices[0].create((float)u1, (float)v0);
        this->vertices[1] = this->vertices[1].create((float)u0, (float)v0);
        this->vertices[2] = this->vertices[2].create((float)u0, (float)v1);
        this->vertices[3] = this->vertices[3].create((float)u1, (float)v1);
    }
    
private:

    Quad(const std::vector<NewVertex>& verts) {
        this->vertices = verts;
    }

};