#pragma once
#include "phys/AABB.hpp"
#include "level/Level.hpp"
#include "level/tile/Tile.hpp"
#include "player/Player.hpp"
#include "Entity.hpp"
#include "HitResult.hpp"
#include <cmath>
#include <vector>
#include <limits>
#include <cfloat>

class Ray {
public:
    float x = 0.0f, y = 0.0f, z = 0.0f;
    float dx = 0.0f, dy = 0.0f, dz = 0.0f;
    
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
    
    // Основной метод raycast с проверкой энтити
    HitResult* trace(Level* level, Player* player, float maxDistance = 5.0f) {
        // Сначала проверяем энтити (они в приоритете)
        float length = std::sqrt(dx * dx + dy * dy + dz * dz);
        if (length < 0.0001f) return nullptr;
        float ndx = dx / length;
        float ndy = dy / length;
        float ndz = dz / length;

        // --- Поиск пересечений с сущностями (как в Java) ---
        HitResult* entityHit = nullptr;
        float entityDist = maxDistance;

        if (player && level->emesh) {
            // Расширенный AABB игрока вдоль луча
            AABB playerBB = player->bb.expand(ndx * maxDistance, ndy * maxDistance, ndz * maxDistance);
            std::vector<Entity*> entities = level->emesh->getEntities(player, playerBB);

            for (Entity* ent : entities) {
                if (!ent->isPickable()) continue;

                // AABB сущности немного расширяем для надёжного касания
                AABB hitbox = ent->bb.grow(0.1f, 0.1f, 0.1f);
                float t = rayAABBIntersection(hitbox);
                if (t >= 0.0f && t < entityDist) {
                    entityDist = t;
                    delete entityHit;
                    entityHit = new HitResult(ent);
                }
            }
        }

        // --- Поиск пересечений с блоками (DDA) ---
        HitResult* blockHit = traceBlocks(level, maxDistance, ndx, ndy, ndz);
        float blockDist = maxDistance;
        if (blockHit) {
            // расстояние до точки пересечения с блоком
            float dx = blockHit->vec.x - x;
            float dy = blockHit->vec.y - y;
            float dz = blockHit->vec.z - z;
            blockDist = std::sqrt(dx * dx + dy * dy + dz * dz);
        }

        // Возвращаем ближайшее
        if (!blockHit) return entityHit;
        if (!entityHit) return blockHit;

        if (entityDist < blockDist) {
            delete blockHit;
            return entityHit;
        } else {
            delete entityHit;
            return blockHit;
        }
    }
    
private:
    // DDA алгоритм для точного обхода воксельной сетки
    HitResult* traceBlocks(Level* level, float maxDistance,
                           float ndx, float ndy, float ndz) {
        int bx = (int)std::floor(x);
        int by = (int)std::floor(y);
        int bz = (int)std::floor(z);

        int stepX = (ndx > 0) ? 1 : -1;
        int stepY = (ndy > 0) ? 1 : -1;
        int stepZ = (ndz > 0) ? 1 : -1;

        float tDeltaX = std::abs(ndx) < 1e-7f ? FLT_MAX : std::abs(1.0f / ndx);
        float tDeltaY = std::abs(ndy) < 1e-7f ? FLT_MAX : std::abs(1.0f / ndy);
        float tDeltaZ = std::abs(ndz) < 1e-7f ? FLT_MAX : std::abs(1.0f / ndz);

        float tMaxX, tMaxY, tMaxZ;
        if (ndx > 0) tMaxX = (bx + 1.0f - x) / ndx;
        else if (ndx < 0) tMaxX = (bx - x) / ndx;
        else tMaxX = FLT_MAX;

        if (ndy > 0) tMaxY = (by + 1.0f - y) / ndy;
        else if (ndy < 0) tMaxY = (by - y) / ndy;
        else tMaxY = FLT_MAX;

        if (ndz > 0) tMaxZ = (bz + 1.0f - z) / ndz;
        else if (ndz < 0) tMaxZ = (bz - z) / ndz;
        else tMaxZ = FLT_MAX;

        float t = 0.0f;
        int lastFace = -1;

        while (t < maxDistance) {
            if (bx < 0 || bx >= level->width ||
                by < 0 || by >= level->depth ||
                bz < 0 || bz >= level->height)
                break;

            int tileId = level->getTile(bx, by, bz);
            if (tileId != 0) {
                Tile* tile = Tile::tiles[tileId];
                if (tile && tile->mayPick()) {
                    // Вычисляем точку пересечения луча с гранью
                    Vec3D hitPoint(
                        x + ndx * t,
                        y + ndy * t,
                        z + ndz * t
                    );
                    return new HitResult(0, bx, by, bz, lastFace, hitPoint);
                }
            }

            // Шаг DDA
            if (tMaxX < tMaxY) {
                if (tMaxX < tMaxZ) {
                    t = tMaxX;
                    tMaxX += tDeltaX;
                    bx += stepX;
                    lastFace = (stepX > 0) ? 4 : 5;
                } else {
                    t = tMaxZ;
                    tMaxZ += tDeltaZ;
                    bz += stepZ;
                    lastFace = (stepZ > 0) ? 2 : 3;
                }
            } else {
                if (tMaxY < tMaxZ) {
                    t = tMaxY;
                    tMaxY += tDeltaY;
                    by += stepY;
                    lastFace = (stepY > 0) ? 0 : 1;
                } else {
                    t = tMaxZ;
                    tMaxZ += tDeltaZ;
                    bz += stepZ;
                    lastFace = (stepZ > 0) ? 2 : 3;
                }
            }
        }
        return nullptr;
    }
    
    // Ray-AABB intersection test
    float rayAABBIntersection(const AABB& box) {
        float tmin = -std::numeric_limits<float>::max();
        float tmax =  std::numeric_limits<float>::max();

        // X slab
        if (std::abs(dx) > 1e-6f) {
            float t1 = (box.x0 - x) / dx;
            float t2 = (box.x1 - x) / dx;
            if (t1 > t2) std::swap(t1, t2);
            tmin = std::max(tmin, t1);
            tmax = std::min(tmax, t2);
        } else if (x < box.x0 || x > box.x1) return -1.0f;

        // Y slab
        if (std::abs(dy) > 1e-6f) {
            float t1 = (box.y0 - y) / dy;
            float t2 = (box.y1 - y) / dy;
            if (t1 > t2) std::swap(t1, t2);
            tmin = std::max(tmin, t1);
            tmax = std::min(tmax, t2);
        } else if (y < box.y0 || y > box.y1) return -1.0f;

        // Z slab
        if (std::abs(dz) > 1e-6f) {
            float t1 = (box.z0 - z) / dz;
            float t2 = (box.z1 - z) / dz;
            if (t1 > t2) std::swap(t1, t2);
            tmin = std::max(tmin, t1);
            tmax = std::min(tmax, t2);
        } else if (z < box.z0 || z > box.z1) return -1.0f;

        if (tmin > tmax || tmax < 0.0f) return -1.0f;
        return tmin >= 0.0f ? tmin : 0.0f; // если начало внутри AABB, считаем 0
    }
};
