#pragma once

class Level;
class Entity;
class LevelLoaderListener;

class MobSpawner {
private:
    Level* level = nullptr;
public:
    MobSpawner(Level* level);
    void spawn(int area, Entity* player, LevelLoaderListener* listener);
};