#include "phys/AABB.hpp"
#include <GL/gl.h>
#include "model/Vec3D.hpp"
#include "HitResult.hpp"

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

bool AABB::xIntersects(const Vec3D* p) const {
    if (!p) return false;
    return p->y >= y0 && p->y <= y1 && p->z >= z0 && p->z <= z1;
}

bool AABB::yIntersects(const Vec3D* p) const {
    if (!p) return false;
    return p->x >= x0 && p->x <= x1 && p->z >= z0 && p->z <= z1;
}

bool AABB::zIntersects(const Vec3D* p) const {
    if (!p) return false;
    return p->x >= x0 && p->x <= x1 && p->y >= y0 && p->y <= y1;
}

HitResult* AABB::clip(Vec3D& start, Vec3D& end) {
    Vec3D* pX0 = start.getXIntersection(&end, x0);
    Vec3D* pX1 = start.getXIntersection(&end, x1);
    Vec3D* pY0 = start.getYIntersection(&end, y0);
    Vec3D* pY1 = start.getYIntersection(&end, y1);
    Vec3D* pZ0 = start.getZIntersection(&end, z0);
    Vec3D* pZ1 = start.getZIntersection(&end, z1);

    if (!xIntersects(pX0)) { delete pX0; pX0 = nullptr; }
    if (!xIntersects(pX1)) { delete pX1; pX1 = nullptr; }
    if (!yIntersects(pY0)) { delete pY0; pY0 = nullptr; }
    if (!yIntersects(pY1)) { delete pY1; pY1 = nullptr; }
    if (!zIntersects(pZ0)) { delete pZ0; pZ0 = nullptr; }
    if (!zIntersects(pZ1)) { delete pZ1; pZ1 = nullptr; }

    Vec3D* best = nullptr;
    int face = -1;

    auto check = [&](Vec3D* p, int f) {
        if (p) {
            if (!best || start.distanceSqrt(p) < start.distanceSqrt(best)) {
                best = p;
                face = f;
            }
        }
    };

    check(pX0, 4);
    check(pX1, 5);
    check(pY0, 0);
    check(pY1, 1);
    check(pZ0, 2);
    check(pZ1, 3);

    // Удаляем все указатели, кроме best
    if (pX0 != best) delete pX0;
    if (pX1 != best) delete pX1;
    if (pY0 != best) delete pY0;
    if (pY1 != best) delete pY1;
    if (pZ0 != best) delete pZ0;
    if (pZ1 != best) delete pZ1;

    if (!best) return nullptr;

    HitResult* result = new HitResult(0, 0, 0, 0, face, *best);
    delete best;
    return result;
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

float AABB::getSize() const {
    float x = this->x1 - this->x0;
    float y = this->y1 - this->y0;
    float z = this->z1 - this->z0;
    return (x+y+z) / 3.0f;
}
