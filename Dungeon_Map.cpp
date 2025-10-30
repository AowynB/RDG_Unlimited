//
// Created by aowyn on 10/19/25.
//
#include "Dungeon_Map.h"
#include "old_dungeon_map/Dungeon_Map.h"

#include <fstream>
#include <iostream>
#include <list>
#include <regex>
#include "Adjacency_List.h"
#include "SVG.h"


constexpr int TILE_SIZE = 50;
constexpr int PASSAGE_SIZE = 10;
constexpr int ROOM_SIZES[4] = {10, 30, 40, 50};

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
    mapFile << SVGHead(mapWidth + TILE_SIZE, mapHeight + TILE_SIZE);

    std::random_device  rd;
    std::mt19937 gen(rd());

    placeExits();
    for (int i = 0; i < rooms.get_size(); i++) {
        mapFile << SVG_tile(i, minX, minY, gen) << std::endl;
    }

    mapFile << SVGEnd();
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
