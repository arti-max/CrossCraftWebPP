#pragma once
#include <GL/gl.h>
#include <cmath>
#include "phys/AABB.hpp"

class Frustum {
private:
    float m_Frustum[6][4];
    float proj[16];
    float modl[16]; 
    float clip[16];

    Frustum() = default;
    
    void normalizePlane(float frustum[6][4], int side);
    void calculateFrustum();

    static Frustum frustum;

public:
    static const int RIGHT = 0;
    static const int LEFT = 1;
    static const int BOTTOM = 2;
    static const int TOP = 3;
    static const int BACK = 4;
    static const int FRONT = 5;
    
    static const int A = 0;
    static const int B = 1;
    static const int C = 2;
    static const int D = 3;

    static Frustum& getFrustum();

    bool pointInFrustum(float x, float y, float z) const;
    bool sphereInFrustum(float x, float y, float z, float radius) const;
    bool cubeFullyInFrustum(float x1, float y1, float z1, float x2, float y2, float z2) const;
    bool cubeInFrustum(float x1, float y1, float z1, float x2, float y2, float z2) const;
    bool isVisible(const AABB& aabb) const;
};
