#pragma once
#include "phys/AABB.hpp"
#include "level/Level.hpp"
#include "level/tile/Tile.hpp"
#include "Player.hpp"
#include "Entity.hpp"
#include "HitResult.hpp"
#include <cmath>
#include <vector>
#include <limits>

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
    
    // Основной метод raycast с проверкой энтити
    HitResult* trace(Level* level, float maxDistance = 5.0f, const std::vector<Entity*>* entities = nullptr) {
        // Сначала проверяем энтити (они в приоритете)
        HitResult* entityHit = nullptr;
        float entityDistance = maxDistance;
        
        if (entities != nullptr) {
            for (Entity* entity : *entities) {
                if (entity == nullptr) continue;
                
                AABB* entityBox = &entity->bb;
                float distance = rayAABBIntersection(entityBox);
                
                if (distance >= 0.0f && distance < entityDistance) {
                    entityDistance = distance;
                    entityHit = new HitResult(1, 0, 0, 0, 0);
                }
            }
        }
        
        // Затем проверяем блоки с DDA алгоритмом
        HitResult* blockHit = traceBlocks(level, maxDistance);
        
        // Возвращаем ближайшее попадание
        if (blockHit == nullptr) {
            return entityHit;
        }
        
        if (entityHit == nullptr) {
            return blockHit;
        }
        
        // Вычисляем расстояние до блока
        float blockDist = std::sqrt(
            (blockHit->x - x) * (blockHit->x - x) +
            (blockHit->y - y) * (blockHit->y - y) +
            (blockHit->z - z) * (blockHit->z - z)
        );
        
        // Возвращаем ближайшее
        if (entityDistance < blockDist) {
            delete blockHit;
            return entityHit;
        } else {
            delete entityHit;
            return blockHit;
        }
    }
    
private:
    // DDA алгоритм для точного обхода воксельной сетки
    HitResult* traceBlocks(Level* level, float maxDistance) {
        // Текущая позиция в сетке
        int blockX = static_cast<int>(std::floor(x));
        int blockY = static_cast<int>(std::floor(y));
        int blockZ = static_cast<int>(std::floor(z));
        
        // Нормализуем направление для корректных вычислений
        float length = std::sqrt(dx * dx + dy * dy + dz * dz);
        if (length < 0.0001f) return nullptr;
        
        float dirX = dx / length;
        float dirY = dy / length;
        float dirZ = dz / length;
        
        // Направление шага (+1 или -1)
        int stepX = dirX > 0 ? 1 : -1;
        int stepY = dirY > 0 ? 1 : -1;
        int stepZ = dirZ > 0 ? 1 : -1;
        
        // Расстояние до следующей границы вокселя в каждом измерении
        float tMaxX, tMaxY, tMaxZ;
        
        // Расстояние, которое нужно пройти в мировых координатах 
        // для перехода на следующий воксель в каждом измерении
        float tDeltaX = (std::abs(dirX) < 0.0001f) ? std::numeric_limits<float>::max() : std::abs(1.0f / dirX);
        float tDeltaY = (std::abs(dirY) < 0.0001f) ? std::numeric_limits<float>::max() : std::abs(1.0f / dirY);
        float tDeltaZ = (std::abs(dirZ) < 0.0001f) ? std::numeric_limits<float>::max() : std::abs(1.0f / dirZ);
        
        // Инициализация tMax - расстояние до первой границы
        if (dirX > 0) {
            tMaxX = (blockX + 1.0f - x) / dirX;
        } else if (dirX < 0) {
            tMaxX = (blockX - x) / dirX;
        } else {
            tMaxX = std::numeric_limits<float>::max();
        }
        
        if (dirY > 0) {
            tMaxY = (blockY + 1.0f - y) / dirY;
        } else if (dirY < 0) {
            tMaxY = (blockY - y) / dirY;
        } else {
            tMaxY = std::numeric_limits<float>::max();
        }
        
        if (dirZ > 0) {
            tMaxZ = (blockZ + 1.0f - z) / dirZ;
        } else if (dirZ < 0) {
            tMaxZ = (blockZ - z) / dirZ;
        } else {
            tMaxZ = std::numeric_limits<float>::max();
        }
        
        float currentDistance = 0.0f;
        int lastFace = -1;
        
        // DDA основной цикл
        while (currentDistance < maxDistance) {
            // Проверка выхода за границы уровня
            if (blockX < 0 || blockX >= level->width ||
                blockY < 0 || blockY >= level->depth ||
                blockZ < 0 || blockZ >= level->height) {
                break;
            }
            
            // Проверяем текущий блок
            int tileId = level->getTile(blockX, blockY, blockZ);
            
            // ОПТИМИЗАЦИЯ: проверяем только если блок не пустой
            if (tileId != 0) {
                Tile* tile = Tile::tiles[tileId];
                
                // НОВОЕ: проверяем mayPick()
                if (tile != nullptr && tile->mayPick()) {
                    // Нашли пикабельный блок
                    return new HitResult(0, blockX, blockY, blockZ, lastFace);
                }
                // Если блок не пикабельный (например жидкость), продолжаем луч
            }
            
            // Переход к следующему вокселю
            // Выбираем ось с минимальным tMax (ближайшая граница)
            if (tMaxX < tMaxY) {
                if (tMaxX < tMaxZ) {
                    // X ось ближайшая
                    currentDistance = tMaxX;
                    tMaxX += tDeltaX;
                    blockX += stepX;
                    lastFace = (stepX > 0) ? 4 : 5; // West : East
                } else {
                    // Z ось ближайшая
                    currentDistance = tMaxZ;
                    tMaxZ += tDeltaZ;
                    blockZ += stepZ;
                    lastFace = (stepZ > 0) ? 2 : 3; // North : South
                }
            } else {
                if (tMaxY < tMaxZ) {
                    // Y ось ближайшая
                    currentDistance = tMaxY;
                    tMaxY += tDeltaY;
                    blockY += stepY;
                    lastFace = (stepY > 0) ? 0 : 1; // Bottom : Top
                } else {
                    // Z ось ближайшая
                    currentDistance = tMaxZ;
                    tMaxZ += tDeltaZ;
                    blockZ += stepZ;
                    lastFace = (stepZ > 0) ? 2 : 3; // North : South
                }
            }
        }
        
        return nullptr;
    }
    
    // Ray-AABB intersection test
    float rayAABBIntersection(AABB* box) {
        // Нормализуем направление
        float length = std::sqrt(dx * dx + dy * dy + dz * dz);
        if (length < 0.0001f) return -1.0f;
        
        float dirX = dx / length;
        float dirY = dy / length;
        float dirZ = dz / length;
        
        // Slab method для ray-AABB intersection
        float t1 = (box->x0 - x) / (dirX + 0.0001f);
        float t2 = (box->x1 - x) / (dirX + 0.0001f);
        float t3 = (box->y0 - y) / (dirY + 0.0001f);
        float t4 = (box->y1 - y) / (dirY + 0.0001f);
        float t5 = (box->z0 - z) / (dirZ + 0.0001f);
        float t6 = (box->z1 - z) / (dirZ + 0.0001f);
        
        float tmin = std::max(std::max(std::min(t1, t2), std::min(t3, t4)), std::min(t5, t6));
        float tmax = std::min(std::min(std::max(t1, t2), std::max(t3, t4)), std::max(t5, t6));
        
        // Нет пересечения если tmax < 0 (box позади луча)
        if (tmax < 0.0f) {
            return -1.0f;
        }
        
        // Нет пересечения если tmin > tmax
        if (tmin > tmax) {
            return -1.0f;
        }
        
        // Возвращаем расстояние до точки входа (или 0 если луч начинается внутри)
        return std::max(0.0f, tmin);
    }
};
