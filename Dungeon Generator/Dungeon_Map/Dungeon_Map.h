//
// Created by aowynbb on 09/06/25.
//
#include <random>
#include <string>
#include <vector>

#ifndef DUNGEON_MAP_H
#define DUNGEON_MAP_H

using std::vector;

namespace DungeonGenerator {

class Dungeon_Map {
private:
    enum direction {north, east, south, west};
    struct Room
    {
        bool visited, generated;
        int position, width, height;
        std::pair<int, int>* relPos;
        std::vector<direction>* exitDirections;
        Room* parent;
    };

    vector<Room*>* rooms;
    vector<vector<Room*>*>* passages;

    int width, height;

public:
    explicit Dungeon_Map(int size);

    void RandomizedDFS() const;
    [[nodiscard]] vector<int> UnvisitedNeighbors(int curr) const;

    void generateMazeSVG() const;
    [[nodiscard]] static std::string SVGLine(int x1, int y1, int x2, int y2) ;
    [[nodiscard]] static std::string SVGRoom(const Room *room, int xOffset, int yOffset) ;

    bool spaceAvailable(Room *currRoom, Room *nextRoom, direction next) const;

    void generateDungeonSVG() const;

    ~Dungeon_Map();
};

} // DungeonGenerator

#endif //DUNGEON_MAP_H
