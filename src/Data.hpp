#pragma once
#include <vector>
#include "level/tile/Tile.hpp"
#include <string>

class Data {
public:
    std::string username;
    std::string sessionid;

    Data(const std::string& username, const std::string& sessionid) {
        this->username = username;
        this->sessionid = sessionid;
    }

    static std::vector<const Tile*> allowedTiles;
    static bool survival;

    static void initAllowedTiles();

    // inline static std::vector<const Tile*> allowedTiles = []() {
    //     std::vector<const Tile*> tiles;
    //     tiles.push_back(Tile::cobblestone);
    //     tiles.push_back(Tile::rock);
    //     tiles.push_back(Tile::dirt);
    //     tiles.push_back(Tile::wood);
    //     tiles.push_back(Tile::log);
    //     tiles.push_back(Tile::leaves);
    //     tiles.push_back(Tile::glass);
    //     tiles.push_back(Tile::sand);
    //     tiles.push_back(Tile::gravel);
    //     tiles.push_back(Tile::redFlower);
    //     tiles.push_back(Tile::yellowFlower);
    //     tiles.push_back(Tile::redMushroom);
    //     tiles.push_back(Tile::brownMushroom);
    //     tiles.push_back(Tile::sponge);
    //     tiles.push_back(Tile::wool1);
    //     tiles.push_back(Tile::wool2);
    //     tiles.push_back(Tile::wool3);
    //     tiles.push_back(Tile::wool4);
    //     tiles.push_back(Tile::wool5);
    //     tiles.push_back(Tile::wool6);
    //     tiles.push_back(Tile::wool7);
    //     tiles.push_back(Tile::wool8);
    //     tiles.push_back(Tile::wool9);
    //     tiles.push_back(Tile::wool10);
    //     tiles.push_back(Tile::wool11);
    //     tiles.push_back(Tile::wool12);
    //     tiles.push_back(Tile::wool13);
    //     tiles.push_back(Tile::wool14);
    //     tiles.push_back(Tile::wool15);
    //     tiles.push_back(Tile::wool16);
    //     tiles.push_back(Tile::goldBlock);
    //     return tiles;
    // }();
};