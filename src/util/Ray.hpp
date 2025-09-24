#pragma once
#include "phys/AABB.hpp"
#include "level/Level.hpp"
#include "Player.hpp"
#include "HitResult.hpp"
#include <cmath>

class Ray {
public:
    float x, y, z;
    float dx, dy, dz;
    
    Ray(float x, float y, float z, float dx, float dy, float dz) 
        : x(x), y(y), z(z), dx(dx), dy(dy), dz(dz) {}
    
    static Ray fromPlayer(Player* player) {
        float xRot = -player->xRot * M_PI / 180.0f;
        float yRot = -player->yRot * M_PI / 180.0f;
        
        float dx = -std::sin(yRot) * std::cos(xRot);
        float dy = std::sin(xRot);  
        float dz = -std::cos(yRot) * std::cos(xRot);
        
        float startY = player->y;
        
        return Ray(player->x, startY, player->z, dx, dy, dz);
    }
    
    HitResult* trace(Level* level, float maxDistance = 5.0f) {
        float step = 0.1f;
        float distance = 0.0f;
        
        while (distance < maxDistance) {
            float px = x + dx * distance;
            float py = y + dy * distance;
            float pz = z + dz * distance;
            
            int blockX = (int)std::floor(px);
            int blockY = (int)std::floor(py);
            int blockZ = (int)std::floor(pz);
            
            if (level->getTile(blockX, blockY, blockZ) != 0) {
                int face = getFaceHit(px - blockX, py - blockY, pz - blockZ);
                return new HitResult(0, blockX, blockY, blockZ, face);
            }
            
            distance += step;
        }
        
        return nullptr;
    }
    
private:
    int getFaceHit(float localX, float localY, float localZ) {
        float minDist = 1.0f;
        int closestFace = 0;
        
        if (localY < minDist) { minDist = localY; closestFace = 0; }
        if ((1.0f - localY) < minDist) { minDist = 1.0f - localY; closestFace = 1; }
        if (localZ < minDist) { minDist = localZ; closestFace = 2; }
        if ((1.0f - localZ) < minDist) { minDist = 1.0f - localZ; closestFace = 3; }
        if (localX < minDist) { minDist = localX; closestFace = 4; }
        if ((1.0f - localX) < minDist) { minDist = 1.0f - localX; closestFace = 5; }
        
        return closestFace;
    }
};
