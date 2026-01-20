#pragma once
#include <vector>
#include <map>
class Player;
class Tile;

class Inventory {
private:
    Player* player;

public:
    Inventory(Player* player);
    int selectedSlot = 0;
    std::vector<int> slots; 

    void addBlockToSlot(int slot, int id);
    int getCurrentBlock();
    int getInSlot(int slot);
    int inInventory(int id, int ignore);
    void pickTile(int id);
    void replaceSlot(Tile* tile);
};