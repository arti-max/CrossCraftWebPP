#pragma once

class AABB {
private:
    float epsilon = 0.0f;

public:
    float x0, y0, z0;
    float x1, y1, z1;

    AABB(float x0, float y0, float z0, float x1, float y1, float z1);

    AABB expand(float xa, float ya, float za) const;

    AABB grow(float xa, float ya, float za) const;

    AABB cloneMove(float xa, float ya, float za) const;

    float clipXCollide(const AABB& c, float xa) const;
    float clipYCollide(const AABB& c, float ya) const; 
    float clipZCollide(const AABB& c, float za) const;

    bool intersects(const AABB& c) const;

    void move(float xa, float ya, float za);
};
