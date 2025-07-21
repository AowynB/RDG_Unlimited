//
// Created by aowynbb on 09/06/25.
//

#include "Dungeon_Map.h"

#include <cmath>
#include <fstream>
#include <random>
#include <regex>
#include <valarray>

#define SVG_HEAD R"(<svg width="X" height="Y" xmlns="http://www.w3.org/2000/svg">)"
#define SVG_LINE R"(<line x1="X1" y1="Y1" x2="X2" y2="Y2" style="stroke:white;stroke-width:2" />)"
#define SVG_RECT R"(<rect width="W" height="H" x="X" y="Y" style="stroke:white;stroke-width:2"/>)"
#define SVG_FOOT R"(</svg>)"

using std::stack, std::string, std::ofstream;

constexpr int CELL_SIZE = 50;
constexpr int PASSAGE_SIZE = 10;
const int ROOM_SIZES[5] = {10, 20, 30, 40, 50};

namespace DungeonGenerator {
    Dungeon_Map::Dungeon_Map(int size)
    {
        rooms = new vector<Room*>;
        passages = new vector<vector<Room*>*>;
        for (int i = 0; i < size; i++)
        {
            rooms->emplace_back(new Room(false, false, i));
            passages->emplace_back(new vector<Room*>());
        }

        width = floor(sqrt(size));
        height = static_cast<int>(ceil(sqrt(size)) + 1);
        RandomizedDFS();
        generateMazeSVG();
        generateDungeonSVG();
    }

    void Dungeon_Map::RandomizedDFS() const
    {
        //initialize the random number generator
        std::random_device rd;
        std::mt19937 random(rd());

        //initialize the stack of rooms to be visited
        stack<Room*> toExplore;

        //add a random room as the start point and mark it as visited
        toExplore.push(rooms->at(random() % rooms->size()));
        toExplore.top()->visited = true;

        while (!toExplore.empty()) //while there are still rooms with potential unvisited neighbors
        {
            //get the next room
            Room* curr = toExplore.top();
            toExplore.pop();

            //check for unvisited neighbors
            vector<int> unvisitedNeighbors = UnvisitedNeighbors(curr->position);

            if (!unvisitedNeighbors.empty()) // if there are unvisited neighbors
            {
                toExplore.push(curr); //push the current room
                //choose a random unvisited neighbor
                Room* next = rooms->at(unvisitedNeighbors.at(random() % unvisitedNeighbors.size()));
                //add a passage between the current room and the next one
                passages->at(curr->position)->emplace_back(next);
                passages->at(next->position)->emplace_back(curr);
                //push the next room
                next->visited = true;
                toExplore.push(next);
            }
        }
    }

    vector<int> Dungeon_Map::UnvisitedNeighbors(int curr) const
    {
        vector<int> unvisitedNeighbors;
        if ((curr - width) >= 0)
        {
            if (!rooms->at(curr - width)->visited)
            {
                unvisitedNeighbors.emplace_back(curr - width);
            }
        }
        if ((curr + width) < rooms->size())
        {
            if (!rooms->at(curr + width)->visited)
            {
                unvisitedNeighbors.emplace_back(curr + width);
            }
        }
        if ((curr % width) > 0)
        {
            if (!rooms->at(curr - 1)->visited)
            {
                unvisitedNeighbors.emplace_back(curr - 1);
            }
        }
        if ((curr % width) < (width - 1) && curr != rooms->size() - 1)
        {
            if (!rooms->at(curr + 1)->visited)
            {
                unvisitedNeighbors.emplace_back(curr + 1);
            }
        }
        return unvisitedNeighbors;
    }

    void Dungeon_Map::generateMazeSVG() const
    {
        ofstream mapFile = ofstream("Dungeon_Maze.svg");
        string header = std::regex_replace(SVG_HEAD, std::regex("X"), std::to_string(width * 10));
        mapFile << std::regex_replace(header, std::regex("Y"), std::to_string(height * 10)) << std::endl;

        int x = 0, y = 0;
        for (Room* room : *rooms)
        {
            bool up = false, right = false, down = false, left = false;
            for (Room* neighbour : *passages->at(room->position))
            {
                if (neighbour->position == room->position + width)
                {
                    up = true;
                }
                else if (neighbour->position == room->position + 1)
                {
                    right = true;
                }
                else if (neighbour->position == room->position - width)
                {
                    down = true;
                }
                else if (neighbour->position == room->position - 1)
                {
                    left = true;
                }
            }
            if (!up)
            {
                mapFile << SVGLine(x, y + 10, x + 10, y + 10) << std::endl;
            }
            if (!right)
            {
                mapFile << SVGLine(x + 10, y, x + 10, y + 10) << std::endl;
            }
            if (!down)
            {
                mapFile << SVGLine(x, y, x + 10, y) << std::endl;
            }
            if (!left)
            {
                mapFile << SVGLine(x, y, x, y + 10) << std::endl;
            }

            if (room->position % width == width - 1)
            {
                y += 10;
                x = 0;
            }
            else
            {
                x += 10;
            }
        }

        mapFile << SVG_FOOT;
    }

    std::string Dungeon_Map::SVGLine(const int x1, const int y1, const int x2, const int y2)
    {
        string lineSVG = std::regex_replace(SVG_LINE, std::regex("X1"), std::to_string(x1));
        lineSVG = std::regex_replace(lineSVG, std::regex("Y1"), std::to_string(y1));
        lineSVG = std::regex_replace(lineSVG, std::regex("X2"), std::to_string(x2));
        return std::regex_replace(lineSVG, std::regex("Y2"), std::to_string(y2));
    }

    std::string Dungeon_Map::SVGRoom(Room* room, int xOffset, int yOffset) {
        std::random_device rd;
        std::mt19937 random(rd());
        const int width = ROOM_SIZES[random() % 5];
        const int height = width;
        int minX = (CELL_SIZE / 2) - (PASSAGE_SIZE / 2) - (width - PASSAGE_SIZE);
        if (minX < 0) { minX = 0; };
        int minY = (CELL_SIZE / 2) - (PASSAGE_SIZE / 2) - (height - PASSAGE_SIZE);
        if (minY < 0) { minY = 0; };
        int maxX = CELL_SIZE - width;
        if (maxX > (CELL_SIZE / 2) - (PASSAGE_SIZE / 2)) { maxX = (CELL_SIZE / 2) - (PASSAGE_SIZE / 2); };
        int maxY = CELL_SIZE - height;
        if (maxY > (CELL_SIZE / 2) - (PASSAGE_SIZE / 2)) { maxY = (CELL_SIZE / 2) - (PASSAGE_SIZE / 2); };

        int roomX = static_cast<int>(random() % (maxX - minX + 1)) + minX;
        //roomX -= roomX % 5;
        int roomY = static_cast<int>(random() % (maxY - minY + 1)) + minY;
        //roomY -= roomY % 5;

        std::string roomSVG;
        int tileX = 50 * (room->relPos->first - xOffset);
        int tileY = 50 * (room->relPos->second - yOffset);
        std::vector<direction>* exits = room->exitDirections;
        // for (const direction exit: *room->exitDirections) {
        //     switch (exit) {
        //         case north:
        //             roomSVG.append(SVGLine(tileX + 20, tileY + roomY + height, tileX + 20, tileY + 50) + "\n");
        //             roomSVG.append(SVGLine(tileX + 30, tileY + roomY + height, tileX + 30, tileY + 50) + "\n");
        //             break;
        //         case east:
        //             roomSVG.append(SVGLine(tileX + roomX + width, tileY + 20, tileX + 50, tileY + 20) + "\n");
        //             roomSVG.append(SVGLine(tileX + roomX + width, tileY + 30, tileX + 50, tileY + 30) + "\n");
        //             break;
        //         case south:
        //             roomSVG.append(SVGLine(tileX + 20, tileY + 0, tileX + 20, tileY + roomY) + "\n");
        //             roomSVG.append(SVGLine(tileX + 30, tileY + 0, tileX + 30, tileY + roomY) + "\n");
        //             break;
        //         case west:
        //             roomSVG.append(SVGLine(tileX + 0, tileY + 20, tileX + roomX, tileY + 20) + "\n");
        //             roomSVG.append(SVGLine(tileX + 0, tileY + 30, tileX + roomX, tileY + 30) + "\n");
        //             break;
        //     }
        // }
        if (find(exits->begin(), exits->end(), north) != exits->end()) {
            roomSVG.append(SVGLine(tileX + 20, tileY + roomY + height, tileX + 20, tileY + 50) + "\n");
            roomSVG.append(SVGLine(tileX + 30, tileY + roomY + height, tileX + 30, tileY + 50) + "\n");
            roomSVG.append(SVGLine(tileX + roomX, tileY + roomY + height, tileX + 20, tileY + roomY + height) + "\n");
            roomSVG.append(SVGLine(tileX + 30, tileY + roomY + height, tileX + roomX + width, tileY + roomY + height) + "\n");
        }
        else {
            roomSVG.append(SVGLine(tileX + roomX, tileY + roomY + height, tileX + roomX + width, tileY + roomY + height) + "\n");
        }

        if (find(exits->begin(), exits->end(), east) != exits->end()) {
            roomSVG.append(SVGLine(tileX + roomX + width, tileY + 20, tileX + 50, tileY + 20) + "\n");
            roomSVG.append(SVGLine(tileX + roomX + width, tileY + 30, tileX + 50, tileY + 30) + "\n");
            roomSVG.append(SVGLine(tileX + roomX + width, tileY + roomY, tileX + roomX + width, tileY + 20) + "\n");
            roomSVG.append(SVGLine(tileX + roomX + width, tileY + 30, tileX + roomX + width, tileY + roomY + height) + "\n");
        }
        else {
            roomSVG.append(SVGLine(tileX + roomX + width, tileY + roomY, tileX + roomX + width, tileY + roomY + height) + "\n");
        }

        if (find(exits->begin(), exits->end(), south) != exits->end()) {
            roomSVG.append(SVGLine(tileX + 20, tileY, tileX + 20, tileY + roomY) + "\n");
            roomSVG.append(SVGLine(tileX + 30, tileY, tileX + 30, tileY + roomY) + "\n");
            roomSVG.append(SVGLine(tileX + roomX, tileY + roomY, tileX + 20, tileY + roomY) + "\n");
            roomSVG.append(SVGLine(tileX + 30, tileY + roomY, tileX + roomX + width, tileY + roomY) + "\n");
        }
        else {
            roomSVG.append(SVGLine(tileX + roomX, tileY + roomY, tileX + roomX + width, tileY + roomY) + "\n");
        }

        if (find(exits->begin(), exits->end(), west) != exits->end()) {
            roomSVG.append(SVGLine(tileX, tileY + 20, tileX + roomX, tileY + 20) + "\n");
            roomSVG.append(SVGLine(tileX, tileY + 30, tileX + roomX, tileY + 30) + "\n");
            roomSVG.append(SVGLine(tileX + roomX, tileY + roomY, tileX + roomX, tileY + 20) + "\n");
            roomSVG.append(SVGLine(tileX + roomX, tileY + 30, tileX + roomX, tileY + roomY + height) + "\n");
        }
        else {
            roomSVG.append(SVGLine(tileX + roomX, tileY + roomY, tileX + roomX, tileY + roomY + height) + "\n");
        }

        // std::string rectSVG = std::regex_replace(SVG_RECT, std::regex("W"), std::to_string(width));
        // rectSVG = std::regex_replace(rectSVG,std::regex("H"), std::to_string(height));
        // rectSVG = std::regex_replace(rectSVG,std::regex("X"), std::to_string(roomX + tileX));
        // rectSVG = std::regex_replace(rectSVG,std::regex("Y"), std::to_string(roomY + tileY));
        // roomSVG.append(rectSVG + "\n");

        return roomSVG;
    }

    bool Dungeon_Map::spaceAvailable(Room *currRoom, Room *nextRoom, const direction next) const {
        auto proposedSpace = *currRoom->relPos;
        switch (next) {
            case north:
                proposedSpace.second +=1;
                break;
            case east:
                proposedSpace.first +=1;
                break;
            case south:
                proposedSpace.second -=1;
                break;
            case west:
                proposedSpace.first -=1;
        }

        for (const Room* room : *rooms) {
            if (room->relPos != nullptr && *room->relPos == proposedSpace){
                return false;
            }
        }

        if (currRoom->exitDirections == nullptr) {
            currRoom->exitDirections = new std::vector<direction>();
        }
        if (nextRoom->exitDirections == nullptr) {
            nextRoom->exitDirections = new std::vector<direction>();
        }

        currRoom->exitDirections->push_back(next);
        switch (next) {
            case north:
                nextRoom->exitDirections->push_back(south);
                break;
            case east:
                nextRoom->exitDirections->push_back(west);
                break;
            case south:
                nextRoom->exitDirections->push_back(north);
                break;
            case west:
                nextRoom->exitDirections->push_back(east);
        }
        nextRoom->relPos = new std::pair<int, int>(proposedSpace.first, proposedSpace.second);
        return true;
    }

    void Dungeon_Map::generateDungeonSVG() const {
        std::random_device rd;
        std::mt19937 random(rd());

        std::stack<Room*> toGenerate;
        rooms->at(0)->relPos = new std::pair<int,int>(0, 0);
        toGenerate.push(rooms->at(0));

        while (!toGenerate.empty()) {
            Room* curroom = toGenerate.top();
            toGenerate.pop();

            if (!curroom->generated) {
                curroom->generated = true;
                for (Room* neighbour : *passages->at(curroom->position)) {
                    if (!neighbour->generated) {
                        bool availableSpace = false;
                        while (!availableSpace) {
                            auto next = static_cast<direction>(random() % 4);
                            if (spaceAvailable(curroom, neighbour, next)) {
                                availableSpace = true;
                            }
                        }
                        toGenerate.push(neighbour);
                    }
                }
            }
        }

        int minX = 0, minY = 0, maxX = 0, maxY = 0;
        for (Room* room : *rooms) {
            if (room->relPos->first > maxX) {
                maxX = room->relPos->first;
            }
            if (room->relPos->second > maxY) {
                maxY = room->relPos->second;
            }
            if (room->relPos->first < minX) {
                minX = room->relPos->first;
            }
            if (room->relPos->second < minY) {
                minY = room->relPos->second;
            }
        }

        int width = (maxX - minX) * CELL_SIZE;
        int height = (maxY - minY) * CELL_SIZE;

        ofstream mapFile = ofstream("Dungeon_Map.svg");
        string header = std::regex_replace(SVG_HEAD, std::regex("X"), std::to_string(width + CELL_SIZE));
        mapFile << std::regex_replace(header, std::regex("Y"), std::to_string(height + CELL_SIZE)) << std::endl;

        for (Room* room : *rooms) {
            mapFile << SVGRoom(room, minX, minY) << std::endl;
        }

        mapFile << SVG_FOOT;
    }

    Dungeon_Map::~Dungeon_Map() {
        for (Room* room : *rooms) {
            delete room->relPos;
            delete room;
        }
        delete rooms;
        for (const vector<Room*>* passage: *passages) {
            delete passage;
        }
        delete passages;
    }
} // DungeonGenerator