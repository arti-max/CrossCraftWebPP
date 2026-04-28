#pragma once
#include <vector>
#include <map>
class Player;
class Tile;

class Inventory {
private:
    Player* player;

public:
    static const int POP_TIME_DURATION = 5;

    Inventory(Player* player);
    int selectedSlot = 0;
    std::vector<int> slots; 
    std::vector<int> count;
    std::vector<int> popTime;

    void addBlockToSlot(int slot, int id);
    int getCurrentBlock();
    int getInSlot(int slot);
    int inInventory(int id, int ignore=-12);
    void pickTile(int id);
    void replaceSlot(Tile* tile);
    bool addItem(int id);
    bool removeItem(int id);
    void tick();
};