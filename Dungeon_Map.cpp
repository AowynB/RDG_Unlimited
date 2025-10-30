//
// Created by aowyn on 10/19/25.
//
#include "Dungeon_Map.h"
#include "old_dungeon_map/Dungeon_Map.h"

#include <fstream>
#include <iostream>
#include <list>
#include <queue>
#include <regex>
#include "Adjacency_List.h"

#define SVG_HEAD R"(<svg width="X" height="Y" xmlns="http://www.w3.org/2000/svg">)"
#define SVG_LINE R"(<line x1="X1" y1="Y1" x2="X2" y2="Y2" style="stroke:white;stroke-width:2" />)"
#define SVG_RECT R"(<rect width="W" height="H" x="X" y="Y" style="stroke:white;stroke-width:2"/>)"
#define SVG_FOOT R"(</svg>)"

constexpr int TILE_SIZE = 50;
constexpr int PASSAGE_SIZE = 10;
constexpr int ROOM_SIZES[4] = {10, 30, 40, 50};

std::string Dungeon_Map::SVGLine(const int x1, const int y1, const int x2, const int y2) {
    std::string lineSVG = regex_replace(SVG_LINE, std::regex("X1"), std::to_string(x1));
    lineSVG = regex_replace(lineSVG, std::regex("Y1"), std::to_string(y1));
    lineSVG = regex_replace(lineSVG, std::regex("X2"), std::to_string(x2));
    return regex_replace(lineSVG, std::regex("Y2"), std::to_string(y2));
}

void Dungeon_Map::validate_dungeon_layout() {

}

Dungeon_Map::Dungeon_Map(const int size) {
    rooms = Adjacency_List<tile>();

    for (int i = 0; i < size; i++) {
        auto temp = tile(0, 0, i);
        rooms.add_vertex(temp);
    }
}

void Dungeon_Map::generate_dungeon_layout(std::mt19937 random_number_generator) {
    //define an edge
    struct edge {
        std::pair<int, int> origin;
        std::pair<int, int> target;

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


    while (num_tiles > 0) {
        auto [origin, target] = unexplored_edges[random_number_generator() % unexplored_edges.size()];
        map_tiles.add_vertex(target);
        map_tiles.add_edge(map_tiles.get_index_of(origin), map_tiles.get_size() - 1);

        while (
            std::ranges::find_if(unexplored_edges, [target](const edge& UE){ return UE.target == target;})
            != unexplored_edges.end()) {
            unexplored_edges.erase(std::ranges::find_if(unexplored_edges, [target](const edge &UE) {
                return UE.target == target;
            }));
        }

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

        num_tiles--;
    }

    for (int i = 0; i < rooms.get_size(); i++) {
        rooms.get_vertex(i).relative_position = map_tiles.get_vertex(i);
        for (auto j : map_tiles.get_edges(i)) {
            rooms.add_edge(i, j);
        }
    }
}


void Dungeon_Map::placeExits(){
    for(int i = 0; i < rooms.get_size(); i++) {
        auto &curr = rooms.get_vertex(i);
        for(const int neighbour_index : rooms.get_edges(i)){
            const auto neighbour = rooms.get_vertex(neighbour_index);
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

void Dungeon_Map::generate_maze_svg()
{
    auto mapFile = std::ofstream("Dungeon_Maze.svg"); //open the file to write to
    int width = floor(sqrt(rooms.get_size()));
    int height = static_cast<int>(ceil(sqrt(rooms.get_size())) + 1);

    //Write the SVG header to the file
    mapFile << std::regex_replace(
        std::regex_replace(SVG_HEAD, std::regex("X"), std::to_string(width * 10)),
                            std::regex("Y"), std::to_string(height * 10)) << std::endl;

    //define x and y values to track location on the resulting image
    int x = 0, y = 0;

    //for each room
    for (int i = 0; i < rooms.get_size(); i++)
    {
        tile curr = rooms.get_vertex(i);
        bool up = false, right = false, down = false, left = false;
        for (int neighbour : rooms.get_edges(curr.index)){
            if (neighbour == curr.index + width) {up = true;}
            else if (neighbour == curr.index + 1) {right = true;}
            else if (neighbour == curr.index - width) {down = true;}
            else if (neighbour == curr.index - 1) {left = true;}
        }
        if (!up){ mapFile << SVGLine(x, y + 10, x + 10, y + 10) << std::endl; }
        if (!right){ mapFile << SVGLine(x + 10, y, x + 10, y + 10) << std::endl; }
        if (!down){ mapFile << SVGLine(x, y, x + 10, y) << std::endl; }
        if (!left){ mapFile << SVGLine(x, y, x, y + 10) << std::endl; }

        if (curr.index % width == width - 1) {y += 10; x = 0;}
        else {x += 10;}
    }

    mapFile << SVG_FOOT;
}

void Dungeon_Map::generate_dungeon_svg() {
    int minX = 0, minY = 0, maxX = 0, maxY = 0;
    for (int i = 0; i < rooms.get_size(); i++) {
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

    std::cout<< maxX << " " << maxY << std::endl;
    std::cout << minX << " " << minY << std::endl;

    int mapWidth = (maxX - minX) * TILE_SIZE;
    int mapHeight = (maxY - minY) * TILE_SIZE;

    std::ofstream mapFile = std::ofstream("Dungeon_Map.svg");
    std::string header = std::regex_replace(SVG_HEAD, std::regex("X"), std::to_string(mapWidth + TILE_SIZE));
    mapFile << std::regex_replace(header, std::regex("Y"), std::to_string(mapHeight + TILE_SIZE)) << std::endl;

    std::random_device  rd;
    std::mt19937 gen(rd());

    placeExits();
    for (int i = 0; i < rooms.get_size(); i++) {
        mapFile << SVG_tile(i, minX, minY, gen) << std::endl;
    }

    mapFile << SVG_FOOT;
}

std::string Dungeon_Map::SVG_tile(int room_index, const int x_offset, const int y_offset, std::mt19937 random_number_generator) {
    int width = ROOM_SIZES[random() % std::size(ROOM_SIZES)];
    int height = ROOM_SIZES[random() % std::size(ROOM_SIZES)];

    int minX = (TILE_SIZE / 2) - (PASSAGE_SIZE / 2) - (width - PASSAGE_SIZE);
    if (minX < 0) { minX = 0; };
    int minY = (TILE_SIZE / 2) - (PASSAGE_SIZE / 2) - (height - PASSAGE_SIZE);
    if (minY < 0) { minY = 0; };
    int maxX = TILE_SIZE - width;
    if (maxX > (TILE_SIZE / 2) - (PASSAGE_SIZE / 2)) { maxX = (TILE_SIZE / 2) - (PASSAGE_SIZE / 2); };
    int maxY = TILE_SIZE - height;
    if (maxY > (TILE_SIZE / 2) - (PASSAGE_SIZE / 2)) { maxY = (TILE_SIZE / 2) - (PASSAGE_SIZE / 2); };

    int roomX = static_cast<int>(random() % (maxX +- minX + 1)) + minX;
    roomX -= roomX % 5;
    int roomY = static_cast<int>(random() % (maxY - minY + 1)) + minY;
    roomY -= roomY % 5;

    std::string roomSVG;
    const auto room = rooms.get_vertex(room_index);
    const int tileX = 50 * (room.relative_position.first - x_offset);
    const int tileY = 50 * (room.relative_position.second - y_offset);

    if (room.exits[NORTH] == true) {
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

    if (room.exits[EAST] == true) {
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

    if (room.exits[SOUTH] == true) {
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

    if (room.exits[WEST] == true) {
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

std::string Dungeon_Map::SVG_corner(const std::pair<int, int> &start, const std::pair<int, int> &corner, const std::pair<int, int> &end) {
    std::string result;
    result += SVGLine(start.first, start.second, corner.first, corner.second);
    result += SVGLine(corner.first, corner.second, end.first, end.second);
    return result;
}
