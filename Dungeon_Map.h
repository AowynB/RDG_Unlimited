//
// Created by aowyn on 10/19/25.
//

#ifndef RDG_UNLIMITED_DUNGEON_MAP_H
#define RDG_UNLIMITED_DUNGEON_MAP_H
#include "Adjacency_List.h"


class Dungeon_Map {
private:
    enum direction {NORTH, EAST, SOUTH, WEST};

    struct tile {
        int width, height, index;
        bool exits[4] = { false, false, false, false };
        std::pair<int, int> relative_position;
    };

    Adjacency_List<tile> rooms;

    [[nodiscard]] static std::string SVGLine(int x1, int y1, int x2, int y2);

    void validate_dungeon_layout();

    std::string SVG_tile(int room_index, int x_offset, int y_offset, std::mt19937 random_number_generator);
    static std::string SVG_corner(const std::pair<int, int> &start, const std::pair<int, int> &end, const std::pair<int, int> &turn);

public:
    explicit Dungeon_Map(int size);

    void generate_dungeon_layout(std::mt19937 random_number_generator);
    void placeExits();

    void generate_maze_svg();
    void generate_dungeon_svg();
};


#endif //RDG_UNLIMITED_DUNGEON_MAP_H