#include "phys/AABB.hpp"
#include <GL/gl.h>

AABB::AABB(float x0, float y0, float z0, float x1, float y1, float z1)
    : x0(x0), y0(y0), z0(z0), x1(x1), y1(y1), z1(z1) {
}

AABB AABB::expand(float xa, float ya, float za) const {
    float _x0 = x0, _y0 = y0, _z0 = z0;
    float _x1 = x1, _y1 = y1, _z1 = z1;
    if (xa < 0.0f) _x0 += xa;
    if (xa > 0.0f) _x1 += xa;
    if (ya < 0.0f) _y0 += ya;
    if (ya > 0.0f) _y1 += ya;
    if (za < 0.0f) _z0 += za;
    if (za > 0.0f) _z1 += za;
    return AABB(_x0, _y0, _z0, _x1, _y1, _z1);
}

AABB AABB::grow(float xa, float ya, float za) const {
    return AABB(x0 - xa, y0 - ya, z0 - za, x1 + xa, y1 + ya, z1 + za);
}

AABB AABB::cloneMove(float xa, float ya, float za) const {
    return AABB(x0 + xa, y0 + ya, z0 + za, x1 + xa, y1 + ya, z1 + za);
}

float AABB::clipXCollide(const AABB& c, float xa) const {
    if (c.y1 > y0 && c.y0 < y1 && c.z1 > z0 && c.z0 < z1) {
        if (xa > 0.0f && c.x1 <= x0) {
            float max = x0 - c.x1 - epsilon;
            if (max < xa) xa = max;
        }
        if (xa < 0.0f && c.x0 >= x1) {
            float max = x1 - c.x0 + epsilon;
            if (max > xa) xa = max;
        }
    }
    return xa;
}

float AABB::clipYCollide(const AABB& c, float ya) const {
    if (c.x1 > x0 && c.x0 < x1 && c.z1 > z0 && c.z0 < z1) {
        if (ya > 0.0f && c.y1 <= y0) {
            float max = y0 - c.y1 - epsilon;
            if (max < ya) ya = max;
        }
        if (ya < 0.0f && c.y0 >= y1) {
            float max = y1 - c.y0 + epsilon;
            if (max > ya) ya = max;
        }
    }
    return ya;
}

float AABB::clipZCollide(const AABB& c, float za) const {
    if (c.x1 > x0 && c.x0 < x1 && c.y1 > y0 && c.y0 < y1) {
        if (za > 0.0f && c.z1 <= z0) {
            float max = z0 - c.z1 - epsilon;
            if (max < za) za = max;
        }
        if (za < 0.0f && c.z0 >= z1) {
            float max = z1 - c.z0 + epsilon;
            if (max > za) za = max;
        }
    }
    return za;
}

bool AABB::intersects(const AABB& c) const {
    return c.x1 > x0 && c.x0 < x1 &&
           c.y1 > y0 && c.y0 < y1 &&
           c.z1 > z0 && c.z0 < z1;
}

bool AABB::intersects(float x0, float y0, float z0, float x1, float y1, float z1) const {
    return x1 > this->x0 && x0 < this->x1 &&
           y1 > this->y0 && y0 < this->y1 &&
           z1 > this->z0 && z0 < this->z1;
}

void AABB::move(float xa, float ya, float za) {
    x0 += xa; y0 += ya; z0 += za;
    x1 += xa; y1 += ya; z1 += za;
}

void AABB::render() const {
    glBegin(GL_LINE_LOOP);
    glVertex3f(x0, y1, z0);
    glVertex3f(x1, y1, z0);
    glVertex3f(x1, y1, z1);
    glVertex3f(x0, y1, z1);
    glEnd();
    glBegin(GL_LINE_LOOP);
    glVertex3f(x0, y0, z0);
    glVertex3f(x1, y0, z0);
    glVertex3f(x1, y0, z1);
    glVertex3f(x0, y0, z1);
    glEnd();
    glBegin(GL_LINES);
    glVertex3f(x0, y0, z0); glVertex3f(x0, y1, z0);
    glVertex3f(x1, y0, z0); glVertex3f(x1, y1, z0);
    glVertex3f(x1, y0, z1); glVertex3f(x1, y1, z1);
    glVertex3f(x0, y0, z1); glVertex3f(x0, y1, z1);
    glEnd();
}