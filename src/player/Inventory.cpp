#include "player/Player.hpp"
#include "level/tile/Tile.hpp"
#include "player/Inventory.hpp"


Inventory::Inventory(Player* p) {
    this->player = p;
    this->slots.resize(9);

    addBlockToSlot(0, Tile::rock->id);
    addBlockToSlot(1, Tile::sponge->id);
    addBlockToSlot(2, Tile::dirt->id);
    addBlockToSlot(3, Tile::wood->id);
    addBlockToSlot(4, Tile::bush->id);
    addBlockToSlot(5, Tile::log->id);
    addBlockToSlot(6, Tile::leaves->id);
    addBlockToSlot(7, Tile::glass->id);
    addBlockToSlot(8, Tile::gravel->id);
}

void Inventory::addBlockToSlot(int slot, int id) {
    this->slots[slot] = id;
}

int Inventory::getCurrentBlock() {
    return this->slots[this->selectedSlot];
}

