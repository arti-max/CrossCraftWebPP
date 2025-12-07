#pragma once
#include <vector>
#include <map>
class Player;

class Inventory {
private:
    Player* player;

public:
    Inventory(Player* player);
    int selectedSlot = 0;
    std::vector<int> slots; 

    void addBlockToSlot(int slot, int id);
    int getCurrentBlock();
};