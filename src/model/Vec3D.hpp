#pragma once
#include <cmath>
#include <string>

class Vec3D {
public:
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    Vec3D() : x(0), y(0), z(0) {}
    Vec3D(float x, float y, float z) {
        this->x = x;
        this->y = y;
        this->z = z;
    }

    Vec3D subtract(const Vec3D& vec) const {
        return Vec3D(x - vec.x, y - vec.y, z - vec.z);
    }

    Vec3D normalize() {
        float n = std::sqrt(this->x * this->x + this->y * this->y + this->z * this->z);
        if (n == 0) return Vec3D(0, 0, 0);
        return Vec3D(this->x / n, this->y / n, this->z / n);
    }

    Vec3D add(float x, float y, float z) {
        return Vec3D(this->x + x, this->y + y, this->z + z);
    }

    float distance(Vec3D* vec) {
        float xd = vec->x - this->x;
        float yd = vec->y - this->y;
        float zd = vec->z - this->z;
        return std::sqrt(x*x + y*y + z*z);
    }

    float distanceSqrt(Vec3D* vec) {
        float xd = vec->x - this->x;
        float yd = vec->y - this->y;
        float zd = vec->z - this->z;
        return x*x + y*y + z*z;
    }

    Vec3D* getXIntersection(Vec3D* vec, float x) {
        float xd = vec->x - this->x;
        float yd = vec->y - this->y;
        float zd = vec->z - this->z;
        return xd*xd < 1.0E-7F?nullptr:((x = (x - this->x) / xd) >= 0.0f && x <= 1.0f ? new Vec3D(this->x + xd*x, this->y + yd*x, this->z + zd*x):nullptr);
    }

    Vec3D* getYIntersection(Vec3D* vec, float y) {
        float xd = vec->x - this->x;
        float yd = vec->y - this->y;
        float zd = vec->z - this->z;
        return yd*yd < 1.0E-7F?nullptr:((y = (y - this->y) / yd) >= 0.0f && y <= 1.0f ? new Vec3D(this->x + xd*y, this->y + yd*y, this->z + zd*y):nullptr);
    }

    Vec3D* getZIntersection(Vec3D* vec, float z) {
        float xd = vec->x - this->x;
        float yd = vec->y - this->y;
        float zd = vec->z - this->z;
        return zd*zd < 1.0E-7F?nullptr:((z = (z - this->z) / zd) >= 0.0f && z <= 1.0f ? new Vec3D(this->x + xd*z, this->y + yd*z, this->z + zd*z):nullptr);
    }

    std::string toString() {
        return "(" + std::to_string(this->x) + ", " + std::to_string(this->y) + ", " + std::to_string(this->z) + ")";
    }

    Vec3D cross(const Vec3D& other) const {
        return Vec3D(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        );
    }
};