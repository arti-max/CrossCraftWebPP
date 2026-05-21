#pragma once
#include "model/Vec3D.hpp"

class Entity;
#include "player/Player.hpp"

class HitResult {
public:
    int type = 0;
    int x = 0;
    int y = 0;
    int z = 0; 
    int f = 0;
    int entityPos = 0;
    Vec3D vec;
    Entity* e;

    HitResult(int type, int x, int y, int z, int f, Vec3D tilepos) :
        type(type), x(x), y(y), z(z), f(f), entityPos(0), vec(tilepos.x, tilepos.y, tilepos.z) {}

    HitResult(Entity* e) :
        entityPos(1), e(e) {}

    bool isCloserThan(Player* player, HitResult* o, int editMode) {
        float dist = this->distanceTo(player, 0);
        float dist2 = o->distanceTo(player, 0);
        if (dist < dist2) {
            return true;
        } else {
            dist = this->distanceTo(player, editMode);
            dist2 = o->distanceTo(player, editMode);
            return dist < dist2;
        }
    }
private:
    float distanceTo(Player* player, int editMode){
        int prevX = this->x;
        int prevY = this->y;
        int prevZ = this->z;
        if (editMode == 1) {
            if (this->f == 0) --prevY;
            if (this->f == 1) ++prevY;
            if (this->f == 2) --prevZ;
            if (this->f == 3) ++prevZ;
            if (this->f == 4) --prevX;
            if (this->f == 5) ++prevX;
        }

        float motionX = (float)prevX - player->x;
        float motionY = (float)prevY - player->y;
        float motionZ = (float)prevZ - player->z;
        return motionX*motionX + motionY*motionY + motionZ*motionZ;
    }
};