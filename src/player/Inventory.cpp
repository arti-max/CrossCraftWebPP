#include "player/Player.hpp"
#include "level/tile/Tile.hpp"
#include "player/Inventory.hpp"
#include "Data.hpp"
#include "util/Logger.hpp"

#include <algorithm>

Inventory::Inventory(Player* p) {
    this->player = p;
    this->slots.resize(9);
    this->count.resize(9);
    this->popTime.resize(9);

    if (Data::survival == false) {
        addBlockToSlot(0, Tile::rock->id);
        addBlockToSlot(1, Tile::cobblestone->id);
        addBlockToSlot(2, Tile::dirt->id);
        addBlockToSlot(3, Tile::wood->id);
        addBlockToSlot(4, Tile::log->id);
        addBlockToSlot(5, Tile::leaves->id);
        addBlockToSlot(6, Tile::bush->id);
        addBlockToSlot(7, Tile::yellowFlower->id);
        addBlockToSlot(8, Tile::redFlower->id);
    } else {
        for (int i = 0; i < 9; ++i) {
            this->slots[i] = -1; // empty
            this->count[i] = 0;
            this->popTime[i] = 0;
        }
    }
}

void Inventory::addBlockToSlot(int slot, int id) {
    this->slots[slot] = id;
}

int Inventory::getCurrentBlock() {
    return this->slots[this->selectedSlot];
}

int Inventory::getInSlot(int slot) {
    return this->slots[slot];
}

int Inventory::inInventory(int id, int ignore) {
    for (int i = 0; i < this->slots.size(); i++) {
        if (i != ignore && this->slots[i] == id) return i;
    }
    return -1;
}

void Inventory::pickTile(int tileId) {
    int inInv = this->inInventory(tileId, -1);
    if (inInv >= 0) {
        this->selectedSlot = inInv;
    } else {
        Tile* targetTile = Tile::tiles[tileId];
        bool isAllowed = std::find(Data::allowedTiles.begin(), Data::allowedTiles.end(), targetTile) != Data::allowedTiles.end();
        // Logger::logf(PREFIX_DEBUG, "PickTile: ID=%d, Allowed=%d\n", tileId, isAllowed);
        if (tileId > 0 && isAllowed) {
            this->replaceSlot(targetTile);
        }
    }
} 

void Inventory::replaceSlot(Tile* tile) {
    if (tile != nullptr) {
        int slot = this->inInventory(tile->id, -1);
        if (slot >= 0) {
            this->slots[slot] = this->slots[this->selectedSlot];
        }

        this->slots[this->selectedSlot] = tile->id;
    }
}

bool Inventory::addItem(int id) {
    int slot = this->inInventory(id);
    if (slot < 0) {
        slot = this->inInventory(-1); // get any empty slot
    }

    if (slot < 0) {
        return false;
    } else if (this->count[slot] >= 99) {
        return false;
    } else {
        this->slots[slot] = id;
        ++this->count[slot];
        this->popTime[slot] = POP_TIME_DURATION;
        return true;
    }
}

bool Inventory::removeItem(int id) {
    int slot = this->inInventory(id);
    if (slot < 0) {
        return false;
    } else {
        if (--this->count[slot] <= 0) {
            this->slots[slot] = -1;
        }

        return true;
    }
}

void Inventory::tick() {
    for (int i = 0; i < 9; ++i) {
        if (this->popTime[i] > 0) {
            --this->popTime[i];
        }
    }
}