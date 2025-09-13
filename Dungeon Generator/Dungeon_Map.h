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
enum direction {north, east, south, west};

class Dungeon_Map {
private:
    struct Room
    {
        bool visited, generated;
        int position, width, height;
        std::pair<int, int> relPos = {0, 0};
        std::vector<direction> exits = std::vector<direction>();
        std::string SVGString;
        std::string description;
    };

    vector<Room*> rooms;
    vector<vector<Room*>> passages;

    int width, height;

    [[nodiscard]] vector<int> UnvisitedNeighbors(int curr) const;
    [[nodiscard]] static std::string SVGLine(int x1, int y1, int x2, int y2);
    [[nodiscard]] static std::string SVGRoom(Room *room, int xOffset, int yOffset);
    bool spaceAvailable(const Room *currRoom, Room *nextRoom, direction next) const;
    static void placeRoom(Room *currRoom, Room *nextRoom, direction next) ;
    static std::string describeExits(const Room *room);
public:
    explicit Dungeon_Map(int size);

    void RandomizedDFS();

    void generateMazeSVG() const;

    void generateDungeonSVG() const;

    void generateDungeonDescription();

    ~Dungeon_Map();
};

} // DungeonGenerator

#endif //DUNGEON_MAP_H
