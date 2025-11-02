//
// Created by aowyn on 10/19/25.
//
#include "Dungeon_Map.h"

#include <fstream>
#include <iostream>
#include <list>
#include <regex>
#include "../Helper_Classes_&_Files/Adjacency_List.h"
#include "../Helper_Classes_&_Files/SVG/SVG.h"

/* CONSTANTS */
constexpr int TILE_SIZE = 50; //the side length of each square tile
constexpr int PASSAGE_SIZE = 10; //the width (wall to wall) of each passage
constexpr int ROOM_SIZES[4] = {10, 30, 40, 50}; //An array of possible sizes of Square rooms

/* METHOD DEFINITIONS */
/**
 * The constructor Initializes @var rooms as an Adjacency_List of tiles then it iterates from 0 - size adding a new tile
 * to rooms each iteration. Each tile is a default without a defined exits or relative position as these are set by later
 * functions
 * @param size
 */
Dungeon_Map::Dungeon_Map(const int size) {
    rooms = Adjacency_List<tile>();

    for (int i = 0; i < size; i++) {
        auto temp = tile();
        rooms.add_vertex(temp);
    }
}

/**
 * generate_dungeon_layout utilizes the basic concepts of randomized depth first search to generate a random dungoen layout.
 * the algorithm can be summerized as randomized depth first search over an unbounded plane limited by the number of tiles
 * in @var rooms.
 *
 * the algorithm is as follows:
 *      Create @var map_tiles, an adjacency list of X,Y coordinate pairs;
 *      Create @var unexplored_edges vector of edges that have not been explored;
 *
 *      add the origin(0,0) to map_tiles and all of its direct neighbors to unexplored_edges;
 *      while( the number of rooms to place is greater than 0 ){
 *          Select a random edge in unexplored_edges;
 *          add the target tile of that edge to map_tiles;
 *          add an edge in map_tiles between the two tiles connected by the selected edge;
 *
 *          remove all additional edges from unexplored_edges that go to the target tile;
 *          add all edges of target tile that go to an unadded position to unexplored_edges;
 *          decrement number of rooms to place;
 *      }
 *
 * The algorithm creates very clustered close to circular dungeon layouts in total but maintains two primary principles:
 *      1. the paths through the dungeon are random
 *      2. if graphed as a set of nodes and edges the dungeon forms a tree from the tile at relative position 0,0 as
 *         root. the graph has no cycles or loops.
 *
 * @param random_number_generator: the pre-seeded random number generator used to select random unexplored edges
 */
void Dungeon_Map::generate_dungeon_layout(std::mt19937 random_number_generator) {
    /**
     * @struct edge defines an edge from cartesian point origin to cartesian point target. the == operator is overloaded
     * to allow for comparrison in the adjacency_list get_index_of() function.
     */
    struct edge {
        std::pair<int, int> origin; //the cartesian coordinate of the origin point
        std::pair<int, int> target; //the cartesian coordinate of the target point

        //overloading the == operator to allow for proper comparison
        bool operator==(const edge &e) const {
            if (origin == e.origin && target == e.target) {
                return true;
            }
            return false;
        }
    };

    //get number of tiles to place after first
    int num_tiles = rooms.get_size() - 1;

    //create an adjacency list to store the selected spaces
    Adjacency_List<std::pair<int, int>> map_tiles;
    //create a list of edges that are unexplored
    std::vector<edge> unexplored_edges;

    //define the first space in the area as being at position (zero, zero)
    std::pair<int, int> root = {0, 0};
    map_tiles.add_vertex(root);

    //add each edge of root to unexplored_edges
    std::pair<int, int> temp = {root.first + 1, root.second};
    edge e = {root, temp};
    unexplored_edges.push_back(e);
    temp = {root.first - 1, root.second};
    e = {root, temp};
    unexplored_edges.push_back(e);
    temp = {root.first, root.second + 1};
    e = {root, temp};
    unexplored_edges.push_back(e);
    temp = {root.first, root.second - 1};
    e = {root, temp};
    unexplored_edges.push_back(e);

    //while there are still tiles to place
    while (num_tiles > 0) {
        //select a random edge in unxplored edges
        auto [origin, target] = unexplored_edges[random_number_generator() % unexplored_edges.size()];
        //add the target point to map_tiles
        map_tiles.add_vertex(target);
        //add an edge in map_tiles between the origin point and the target point
        map_tiles.add_edge(map_tiles.get_index_of(origin), map_tiles.get_size() - 1);

        //while there are still edges in unexplored_edges that point to the target point
        while (
            std::ranges::find_if(unexplored_edges, [target](const edge& UE){ return UE.target == target;})
            != unexplored_edges.end()) {
            //remove those edges from unexplored_edges
            unexplored_edges.erase(std::ranges::find_if(unexplored_edges, [target](const edge &UE) {
                return UE.target == target;
            }));
        }

        //check the positions in each direction from the target edge and add an edge to unexplored edges from target
        //edge to that position if it is unoccupied.
        temp = {target.first + 1, target.second};
        if (map_tiles.get_index_of(temp) == -1) {
            e = {target, temp};
            unexplored_edges.push_back(e);
        }
        temp = {target.first - 1, target.second};
        if (map_tiles.get_index_of(temp) == -1) {
            e = {target, temp};
            unexplored_edges.push_back(e);
        }
        temp = {target.first, target.second + 1};
        if (map_tiles.get_index_of(temp) == -1) {
            e = {target, temp};
            unexplored_edges.push_back(e);
        }
        temp = {target.first, target.second - 1};
        if (map_tiles.get_index_of(temp) == -1) {
            e = {target, temp};
            unexplored_edges.push_back(e);
        }

        //decrement num_tiles;
        num_tiles--;
    }

    //for each room in rooms
    for (int i = 0; i < rooms.get_size(); i++) {
        //set the relative position of that room to be equal to the position at the same index in map_tiles
        rooms.get_vertex(i).relative_position = map_tiles.get_vertex(i);
        //for each edge in map_tiles connected to vertex i
        //place an equivalent edge in rooms
        for (const auto j : map_tiles.get_edges(i)) {
            rooms.add_edge(i, j);
        }
    }
}

/**
 * Place exits is a simple function that goes through rooms and ensures that every tile has the correct exit flags set
 * to true. It iterates through every tile in rooms by index and iterates over the tiles edges, for every edge it checks
 * its relative position and sets the appropriate exit flag. if somehow a tile has an exit that is not adjacent in a cardinal
 * direction the function prints the offending edge to the error stream.
 */
void Dungeon_Map::place_exits(){
    //for each tile in rooms
    for(int i = 0; i < rooms.get_size(); i++) {
        //get the tile at index i
        auto &curr = rooms.get_vertex(i);
        //for each neighbor of the tile
        for(const int neighbour_index : rooms.get_edges(i)){
            const auto neighbour = rooms.get_vertex(neighbour_index);
            //determine which direction the exit should be and set the appropriate flag
            if(neighbour.relative_position.second == curr.relative_position.second + 1){
                curr.exits[NORTH] = true;
            }
            else if(neighbour.relative_position.first == curr.relative_position.first + 1){
                curr.exits[EAST] = true;
            }
            else if(neighbour.relative_position.second == curr.relative_position.second - 1){
                curr.exits[SOUTH] = true;
            }
            else if(neighbour.relative_position.first == curr.relative_position.first - 1){
                curr.exits[WEST] = true;
            }
            else {
                std::cerr << neighbour.relative_position.first << "," << neighbour.relative_position.second << "!=" <<
                    curr.relative_position.first << "," << curr.relative_position.second << std::endl;
            }
        }
    }
}

/**
 * generate_dungeon_svg calculates the necessary offsets, and prints all lines to the Dungeon_Map.svg
 * It starts by iterating all the rooms and finding the maximum and minimum values of the relative positions of each room
 * these values are used to calculate the width and height values of the SVG image. In addition, the minimum values are used
 * to calculate the true positions of each tile in the dungeon svg such that they maintain the same relative positions.
 *
 * the function prints the SVG header to Dungeon_Map.svg
 * then it iterates through each room printing the SVG string generated for that room to Dungeon_Map.svg
 * finally it prints the SVG footer to Dungeon_Map.svg
 */
void Dungeon_Map::generate_dungeon_svg(std::mt19937 random_number_generator) {
    //declare and initialize necessary variables
    int minX = 0, minY = 0, maxX = 0, maxY = 0;
    //for every room
    for (int i = 0; i < rooms.get_size(); i++) {
        //update minimum and maximum parameters
        if (rooms.get_vertex(i).relative_position.first > maxX) {
            maxX = rooms.get_vertex(i).relative_position.first;
        }
        if (rooms.get_vertex(i).relative_position.second > maxY) {
            maxY = rooms.get_vertex(i).relative_position.second;
        }
        if (rooms.get_vertex(i).relative_position.first < minX) {
            minX = rooms.get_vertex(i).relative_position.first;
        }
        if (rooms.get_vertex(i).relative_position.second < minY) {
            minY = rooms.get_vertex(i).relative_position.second;
        }
    }

    //calculate the width and height of our Map_SVG
    int mapWidth = (maxX - minX) * TILE_SIZE;
    int mapHeight = (maxY - minY) * TILE_SIZE;

    //open the file
    std::ofstream mapFile = std::ofstream("Dungeon_Map.svg");
    //write the SVG header
    mapFile << SVGHead(mapWidth + TILE_SIZE, mapHeight + TILE_SIZE);

    //write each room to the file
    place_exits();
    for (int i = 0; i < rooms.get_size(); i++) {
        mapFile << SVG_tile(i, minX, minY, random_number_generator) << std::endl;
    }

    //Write the footer and close the file
    mapFile << SVGEnd();
    mapFile.close();
}

/**
 * SVG_tile takes a specified tile and generates the SVG for that tile at the true position. the function starts by
 * generating a random height and width for the room in the tile from @var ROOM_SIZES. if height or width is 10 then the
 * room is considered to be a passage. Using height and width the function calculates the maximum and minimum values of
 * the upper right corner of the room such that the room completely overlaps the centre 10 x 10 square in the middle of
 * the tile. It then uses @var random_number_generator to generate a random position rounded to the nearest 5 pixels that
 * is within the parameters. then the real position of the tile is calculated using the relative position of the tile as
 * well as @var x_offset & @var y_offset.
 *
 * finally the program generates all necessary lines by using the calculated values to create walls.
 * for each cardinal direction the function checks if the tile has an exit in that direction.
 *      if there is an exit in the direction SVG_corner is called to generate two corners that form the wall segments
 *      of the room and the wall segment of the passage centered in the middle 10 pixels of that side.
 *      if there is not an exit the function create a line for the wall on that side of the room.
 *
 * the generated SVG lines are returned as a combined string.
 *
 * @param room_index: the index of the tile to be generated
 * @param x_offset: the integer amount necessary to shift the tile to it's true x position from relative x position
 * @param y_offset: the integer amount necessary to shift the tile to it's true y position from relative y position
 * @param random_number_generator: the pre-seeded random number generator used to select random room placements, widths, and heights
 * @return the SVG for the tile at @param room_index
 */
std::string Dungeon_Map::SVG_tile(const int room_index, const int x_offset, const int y_offset, std::mt19937 random_number_generator) {
    int width = ROOM_SIZES[random_number_generator() % std::size(ROOM_SIZES)];
    int height = ROOM_SIZES[random_number_generator() % std::size(ROOM_SIZES)];

    int minX = (TILE_SIZE / 2) - (PASSAGE_SIZE / 2) - (width - PASSAGE_SIZE);
    if (minX < 0) { minX = 0; };
    int minY = (TILE_SIZE / 2) - (PASSAGE_SIZE / 2) - (height - PASSAGE_SIZE);
    if (minY < 0) { minY = 0; };
    int maxX = TILE_SIZE - width;
    if (maxX > (TILE_SIZE / 2) - (PASSAGE_SIZE / 2)) { maxX = (TILE_SIZE / 2) - (PASSAGE_SIZE / 2); };
    int maxY = TILE_SIZE - height;
    if (maxY > (TILE_SIZE / 2) - (PASSAGE_SIZE / 2)) { maxY = (TILE_SIZE / 2) - (PASSAGE_SIZE / 2); };

    int roomX = static_cast<int>(random_number_generator() % (maxX +- minX + 1)) + minX;
    roomX -= roomX % 5;
    int roomY = static_cast<int>(random_number_generator() % (maxY - minY + 1)) + minY;
    roomY -= roomY % 5;

    std::string roomSVG;
    const auto [exits, relative_position] = rooms.get_vertex(room_index);
    const int tileX = 50 * (relative_position.first - x_offset);
    const int tileY = 50 * (relative_position.second - y_offset);

    if (exits[NORTH] == true) {
           roomSVG.append(SVG_corner(
               {roomX + tileX, roomY + tileY + height},
               {tileX + (TILE_SIZE / 2) - (PASSAGE_SIZE / 2), roomY + tileY + height},
               {tileX + (TILE_SIZE / 2) - (PASSAGE_SIZE / 2), tileY + TILE_SIZE}));
           roomSVG.append(SVG_corner(
               {tileX + (TILE_SIZE / 2) + (PASSAGE_SIZE / 2), tileY + TILE_SIZE},
               {tileX + (TILE_SIZE / 2) + (PASSAGE_SIZE / 2), roomY + tileY + height},
               {roomX + tileX + width,roomY + tileY + height}));
    }
    else {
        roomSVG.append(SVGLine(tileX + roomX, tileY + roomY + height, tileX + roomX + width, tileY + roomY + height) + "\n");
    }

    if (exits[EAST] == true) {
        roomSVG.append(SVG_corner(
              {roomX + tileX + width, roomY + tileY + height},
             {roomX + tileX + width, tileY + (TILE_SIZE / 2) + (PASSAGE_SIZE / 2)},
               {tileX + TILE_SIZE, tileY + (TILE_SIZE / 2) + (PASSAGE_SIZE / 2)}));
        roomSVG.append(SVG_corner(
              {tileX + TILE_SIZE, tileY + (TILE_SIZE / 2) - (PASSAGE_SIZE / 2)},
             {roomX + tileX + width, tileY + (TILE_SIZE / 2) - (PASSAGE_SIZE / 2)},
               {roomX + tileX + width, roomY + tileY}));
    }
    else {
        roomSVG.append(SVGLine(tileX + roomX + width, tileY + roomY, tileX + roomX + width, tileY + roomY + height) + "\n");
    }

    if (exits[SOUTH] == true) {
        roomSVG.append(SVG_corner(
             {roomX + tileX, roomY + tileY},
            {tileX + (TILE_SIZE / 2) - (PASSAGE_SIZE / 2), roomY + tileY},
              {tileX + (TILE_SIZE / 2) - (PASSAGE_SIZE / 2), tileY}));
        roomSVG.append(SVG_corner(
             {tileX + (TILE_SIZE / 2) + (PASSAGE_SIZE / 2), tileY},
            {tileX + (TILE_SIZE / 2) + (PASSAGE_SIZE / 2), roomY + tileY},
              {roomX + tileX + width,roomY + tileY}));
    }
    else {
        roomSVG.append(SVGLine(tileX + roomX, tileY + roomY, tileX + roomX + width, tileY + roomY) + "\n");
    }

    if (exits[WEST] == true) {
        roomSVG.append(SVG_corner(
             {roomX + tileX, roomY + tileY + height},
            {roomX + tileX, tileY + (TILE_SIZE / 2) + (PASSAGE_SIZE / 2)},
              {tileX, tileY + (TILE_SIZE / 2) + (PASSAGE_SIZE / 2)}));
        roomSVG.append(SVG_corner(
             {tileX, tileY + (TILE_SIZE / 2) - (PASSAGE_SIZE / 2)},
            {roomX + tileX, tileY + (TILE_SIZE / 2) - (PASSAGE_SIZE / 2)},
              {roomX + tileX, roomY + tileY}));
    }
    else {
        roomSVG.append(SVGLine(tileX + roomX, tileY + roomY, tileX + roomX, tileY + roomY + height) + "\n");
    }

    return roomSVG;
}
