#pragma once

class Vec3 {
public:
    float x;
    float y;
    float z;
    Vec3(float x, float y, float z) {
        this->x = x;
        this->y = y;
        this->z = z;
    }
    Vec3* interpolateTo(Vec3* t, float partialTicks) {
        float xt = this->x + (t->x - this->x) * partialTicks;
        float yt = this->y + (t->y - this->y) * partialTicks;
        float zt = this->z + (t->z - this->z) * partialTicks;
        return new Vec3(xt, yt, zt);
    }
    void set(float x, float y, float z) {
        this->x = x;
        this->y = y;
        this->z = z;
    }
};