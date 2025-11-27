#pragma once

class Vec3 {
public:
    float x, y, z;
    
    Vec3(float x = 0, float y = 0, float z = 0) : x(x), y(y), z(z) {}
    
    Vec3 interpolateTo(const Vec3& t, float partialTicks) const {
        float xt = this->x + (t.x - this->x) * partialTicks;
        float yt = this->y + (t.y - this->y) * partialTicks;
        float zt = this->z + (t.z - this->z) * partialTicks;
        return Vec3(xt, yt, zt);
    }
};
