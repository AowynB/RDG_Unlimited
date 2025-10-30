//
// Created by aowyn on 10/19/25.
//

#ifndef RDG_UNLIMITED_DUNGEON_MAP_H
#define RDG_UNLIMITED_DUNGEON_MAP_H
#include "Adjacency_List.h"
#include <random>


class Dungeon_Map {
private:
    enum direction {NORTH, EAST, SOUTH, WEST};

    struct tile {
        int width, height, index;
        bool exits[4] = { false, false, false, false };
        std::pair<int, int> relative_position;
    };

    Adjacency_List<tile> rooms;

    void validate_dungeon_layout();

    std::string SVG_tile(int room_index, int x_offset, int y_offset, std::mt19937 random_number_generator);

public:
    explicit Dungeon_Map(int size);

    void generate_dungeon_layout(std::mt19937 random_number_generator);
    void placeExits();

    void generate_dungeon_svg();
};


#endif //RDG_UNLIMITED_DUNGEON_MAP_H