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
#include <algorithm>

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
    
    HitResult* trace(Level* level, Player* player, float maxDistance = 5.0f) {
        float length = std::sqrt(dx * dx + dy * dy + dz * dz);
        if (length < 0.0001f) return nullptr;
        float ndx = dx / length;
        float ndy = dy / length;
        float ndz = dz / length;

        // --- Поиск пересечений с сущностями ---
        HitResult* entityHit = nullptr;
        float entityDist = maxDistance;

        if (player && level->emesh) {
            AABB playerBB = player->bb.expand(ndx * maxDistance, ndy * maxDistance, ndz * maxDistance);
            std::vector<Entity*> entities = level->emesh->getEntities(player, playerBB);

            for (Entity* ent : entities) {
                if (!ent->isPickable()) continue;

                AABB hitbox = ent->bb.grow(0.1f, 0.1f, 0.1f);
                float t = rayAABBIntersection(hitbox);
                if (t >= 0.0f && t < entityDist) {
                    entityDist = t;
                    delete entityHit;
                    entityHit = new HitResult(ent);
                }
            }
        }

        // --- Поиск пересечений с блоками ---
        HitResult* blockHit = traceBlocks(level, maxDistance, ndx, ndy, ndz);
        float blockDist = maxDistance;
        if (blockHit) {
            float bdx = blockHit->vec.x - x;
            float bdy = blockHit->vec.y - y;
            float bdz = blockHit->vec.z - z;
            blockDist = std::sqrt(bdx * bdx + bdy * bdy + bdz * bdz);
        }

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
    // Точный тест пересечения луча с AABB блока с определением нормали/грани
    bool intersectBox(const AABB& box, float ndx, float ndy, float ndz, float& outT, int& outFace) const {
        float tmin = -std::numeric_limits<float>::max();
        float tmax =  std::numeric_limits<float>::max();
        int face = -1;

        // X slab (грани 4 = -X / West, 5 = +X / East)
        if (std::abs(ndx) > 1e-7f) {
            float t1 = (box.x0 - x) / ndx;
            float t2 = (box.x1 - x) / ndx;
            int signFace = (ndx > 0.0f) ? 4 : 5;
            if (t1 > t2) std::swap(t1, t2);

            if (t1 > tmin) {
                tmin = t1;
                face = signFace;
            }
            tmax = std::min(tmax, t2);
        } else if (x < box.x0 || x > box.x1) {
            return false;
        }

        // Y slab (грани 0 = -Y / Bottom, 1 = +Y / Top)
        if (std::abs(ndy) > 1e-7f) {
            float t1 = (box.y0 - y) / ndy;
            float t2 = (box.y1 - y) / ndy;
            int signFace = (ndy > 0.0f) ? 0 : 1;
            if (t1 > t2) std::swap(t1, t2);

            if (t1 > tmin) {
                tmin = t1;
                face = signFace;
            }
            tmax = std::min(tmax, t2);
        } else if (y < box.y0 || y > box.y1) {
            return false;
        }

        // Z slab (грани 2 = -Z / North, 3 = +Z / South)
        if (std::abs(ndz) > 1e-7f) {
            float t1 = (box.z0 - z) / ndz;
            float t2 = (box.z1 - z) / ndz;
            int signFace = (ndz > 0.0f) ? 2 : 3;
            if (t1 > t2) std::swap(t1, t2);

            if (t1 > tmin) {
                tmin = t1;
                face = signFace;
            }
            tmax = std::min(tmax, t2);
        } else if (z < box.z0 || z > box.z1) {
            return false;
        }

        if (tmin > tmax || tmax < 0.0f) return false;

        outT = (tmin >= 0.0f) ? tmin : 0.0f;
        outFace = face;
        return true;
    }

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

        while (t <= maxDistance) {
            if (bx >= 0 && bx < level->width &&
                by >= 0 && by < level->depth &&
                bz >= 0 && bz < level->height) {

                int tileId = level->getTile(bx, by, bz);
                if (tileId != 0) {
                    Tile* tile = Tile::tiles[tileId];
                    if (tile && tile->mayPick()) {
                        // Формируем мировой AABB для текущего блока с учётом его формы
                        AABB box(
                            (float)bx + tile->minX,
                            (float)by + tile->minY,
                            (float)bz + tile->minZ,
                            (float)bx + tile->maxX,
                            (float)by + tile->maxY,
                            (float)bz + tile->maxZ
                        );

                        float hitT = 0.0f;
                        int hitFace = -1;
                        if (intersectBox(box, ndx, ndy, ndz, hitT, hitFace) && hitT <= maxDistance) {
                            Vec3D hitPoint(x + ndx * hitT, y + ndy * hitT, z + ndz * hitT);
                            return new HitResult(0, bx, by, bz, hitFace, hitPoint);
                        }
                    }
                }
            }

            // Шаг DDA к следующему вокселю
            if (tMaxX < tMaxY) {
                if (tMaxX < tMaxZ) {
                    t = tMaxX;
                    tMaxX += tDeltaX;
                    bx += stepX;
                } else {
                    t = tMaxZ;
                    tMaxZ += tDeltaZ;
                    bz += stepZ;
                }
            } else {
                if (tMaxY < tMaxZ) {
                    t = tMaxY;
                    tMaxY += tDeltaY;
                    by += stepY;
                } else {
                    t = tMaxZ;
                    tMaxZ += tDeltaZ;
                    bz += stepZ;
                }
            }
        }
        return nullptr;
    }
    
    // Ray-AABB intersection test для сущностей
    float rayAABBIntersection(const AABB& box) {
        float tmin = -std::numeric_limits<float>::max();
        float tmax =  std::numeric_limits<float>::max();

        if (std::abs(dx) > 1e-6f) {
            float t1 = (box.x0 - x) / dx;
            float t2 = (box.x1 - x) / dx;
            if (t1 > t2) std::swap(t1, t2);
            tmin = std::max(tmin, t1);
            tmax = std::min(tmax, t2);
        } else if (x < box.x0 || x > box.x1) return -1.0f;

        if (std::abs(dy) > 1e-6f) {
            float t1 = (box.y0 - y) / dy;
            float t2 = (box.y1 - y) / dy;
            if (t1 > t2) std::swap(t1, t2);
            tmin = std::max(tmin, t1);
            tmax = std::min(tmax, t2);
        } else if (y < box.y0 || y > box.y1) return -1.0f;

        if (std::abs(dz) > 1e-6f) {
            float t1 = (box.z0 - z) / dz;
            float t2 = (box.z1 - z) / dz;
            if (t1 > t2) std::swap(t1, t2);
            tmin = std::max(tmin, t1);
            tmax = std::min(tmax, t2);
        } else if (z < box.z0 || z > box.z1) return -1.0f;

        if (tmin > tmax || tmax < 0.0f) return -1.0f;
        return tmin >= 0.0f ? tmin : 0.0f;
    }
};