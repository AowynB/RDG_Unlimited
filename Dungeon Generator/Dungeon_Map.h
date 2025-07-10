//
// Created by aowynbb on 09/06/25.
//
#include <stack>
#include <string>
#include <vector>

#ifndef DUNGEON_MAP_H
#define DUNGEON_MAP_H

using std::vector;

namespace DungeonGenerator {

class Dungeon_Map {
private:
    struct Room
    {
        bool visited;
        int position;
    };

    vector<Room*>* rooms;
    vector<vector<Room*>*>* passages;

    int width, height;

public:
    Dungeon_Map(int size);

    void RandomizedDFS() const;
    vector<int> UnvisitedNeighbors(int curr) const;

    void generateSVG() const;
    std::string SVGLine(int x1, int y1, int x2, int y2) const;
};

} // DungeonGenerator

#endif //DUNGEON_MAP_H
