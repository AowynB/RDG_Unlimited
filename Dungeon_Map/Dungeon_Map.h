//
// Created by aowyn on 10/19/25.
//

#ifndef RDG_UNLIMITED_DUNGEON_MAP_H
#define RDG_UNLIMITED_DUNGEON_MAP_H
#include "../Helper_Classes_&_Files/Adjacency_List.h"
#include <random>

/**
 * Dungeon Map is a class that contains the necessary information and methods to contruct a randomized dungeon of N-tiles
 * and print it to an SVG file. the class uses an adjacency list of tiles to store the relative position and connection
 * points of each tile.
 *
 * Dpendencies:
 *      - Adjacency_List.h
 *      - SVG.h
 *
 * Types:
 *      - @struct tile
 *      - @enum direction
 *
 * Attributes:
 *      - @var rooms: an adjacency list of tiles that is used to store the connections between tiles and corridors
 */
class Dungeon_Map {
private:
    /**
     * The direction enum stores the values NORTH, EAST, WEST, and SOUTH. used primarily for readability purposes the
     * direction enum specifies a direction in 2-dimensional space corresponding to a specific change in X or Y. Functionality
     * of this relationship is implemented in functions and can be generalized as follows:
     *      - NORTH: indicates a positive change in the Y axis
     *      - EAST: indicates a positive change in the X axis
     *      - SOUTH: indicates a negative change in the Y axis
     *      - WEST: indicates a negative change in the X axis
     */
    enum direction {NORTH, EAST, SOUTH, WEST};

    /**
     * The tile structure is a composition of all required information about a tiles placement relative to other tiles
     * including exit directions.
     * @var exits[4]: an array of four boolean values determining the direction of passages connected to this tile.
     * @var relative_position: this pair of integer values determines the position of the tile relative to the starting
     *      tile which is located at (0, 0). the values of relative_position.first && relative_position.second can be
     *      negative.
     */
    struct tile {
        bool exits[4] = { false, false, false, false };
        std::pair<int, int> relative_position;
    };

    //The Adjacency list containing all tiles in the dungeon.
    Adjacency_List<tile> rooms;

    /**
     * SVG_tile generates the SVG strings that represents a specific_tile in the grid and returns it. any rooms
     * are generated in random part of the tile rounded to the nearest multiple of 5 bits on the vertical and horizontal
     * axis. in addition to the rooms the program generates exiting passages from rooms to the edge of the tile. if the
     * tile contains a passage the room is considered to have the same width and height as a passage and is centered in
     * the exact center of the tile.
     * @param room_index: the index of the tile in @var rooms
     * @param x_offset: the required positional shift on the horizontal axis to maintain the relative position to other
     *                  tiles while ensuring that all values are non-negative
     * @param y_offset: the required positional shift on the vertical axis to maintain the relative position to other
     *                  tiles while ensuring that all values are non-negative
     * @param random_number_generator: the pre-seeded random number generator that SVG_tiles uses to generate the room
     *                  within the tile.
     * @return a string containing all svg tags required to display the tile
     */
    std::string SVG_tile(int room_index, int x_offset, int y_offset, std::mt19937 random_number_generator);

    /**
     * place exits iterates through the tiles in rooms and for each one sets the appropriate exit flags to true such
     * that all rooms have the required exit flags for SVG_tile to connect draw passages in the correct directions.
     */
    void place_exits();
public:
    /**
     * The constructor for Dungeon_Map initializes rooms and populates it with the specified number of tiles.
     * @param size: the number of tiles in the dungeon
     */
    explicit Dungeon_Map(int size);

    /**
     * generate_dungeon_layout generates the general structure and layout of tiles relative to each-other including
     * determining which tiles are connected to each-other. direction of passages are inferred later on by place_exits()
     * based on the relative positions of tiles that are connected to each-other. The algorithm starts with a single room
     * and continues placing rooms next to generated rooms until it has generated a room for each tile in rooms.
     * @param random_number_generator: the pre-seeded random number generator used to select random unexplored edges
     */
    void generate_dungeon_layout(std::mt19937 random_number_generator);

    /**
     * generate_dungeon_svg takes the structure of rooms and itterates through it to generate a random layout for each
     * tile including a room or corridor and exits. This function also calculates the necessary width and hight for
     * the SVG header. All the SVG information is written to Dungoen_Map.svg
     */
    void generate_dungeon_svg(std::mt19937 random_number_generator);
};


#endif //RDG_UNLIMITED_DUNGEON_MAP_H