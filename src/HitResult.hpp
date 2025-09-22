#pragma once

class HitResult {
public:
    int type, x, y, z, f;
    HitResult(int type, int x, int y, int z, int f) :
        type(type), x(x), y(y), z(z), f(f) {}

    bool isClotherThan(HitResult o, int editMode) { // TODO: Player

    }
private:
    float distanceTo(int editMode){ // TODO: Player

    }
};