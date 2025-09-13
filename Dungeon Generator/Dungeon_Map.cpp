//
// Created by aowynbb on 09/06/25.
//

#include "Dungeon_Map.h"

#include <cmath>
#include <fstream>
#include <iostream>
#include <random>
#include <regex>

#include "Random_Table.h"

#define SVG_HEAD R"(<svg width="X" height="Y" xmlns="http://www.w3.org/2000/svg">)"
#define SVG_LINE R"(<line x1="X1" y1="Y1" x2="X2" y2="Y2" style="stroke:white;stroke-width:2" />)"
#define SVG_RECT R"(<rect width="W" height="H" x="X" y="Y" style="stroke:white;stroke-width:2"/>)"
#define SVG_FOOT R"(</svg>)"
#define ROOM_DESCRIPTION R"(This room has N exit/s: ED. The room is a RU. The rooms condition is "RC")"
#define DOOR_DESCRIPTION R"(A DT to the DIR)"

using std::stack, std::string, std::ofstream, std::ifstream, std::regex, std::regex_replace;

constexpr int CELL_SIZE = 50;
constexpr int PASSAGE_SIZE = 10;
constexpr int ROOM_SIZES[4] = {10, 30, 40, 50};
const auto DOOR_TABLE = Table::Random_Table("Table_Files/doorType.txt");
const auto ROOM_CONDITION_TABLE = Table::Random_Table("Table_Files/CurrentRoomState.txt");
const auto DUNGEON_TYPE_TABLE = Table::Random_Table("Table_Files/DungeonType.txt");
const auto GENERAL_ROOMS_TABLE = Table::Random_Table("Table_Files/DungeonTypes/General.txt");

namespace DungeonGenerator {
    Dungeon_Map::Dungeon_Map(const int size) {
        rooms = vector<Room*>();
        passages = vector<vector<Room*>>();

        for (int i = 0; i < size; i++)
        {
            rooms.emplace_back(new Room(false, false, i));
            passages.emplace_back();
        }

        width = floor(sqrt(size));
        height = static_cast<int>(ceil(sqrt(size)) + 1);
    }

    void Dungeon_Map::RandomizedDFS() {
        std::random_device rd;
        std::mt19937 random(rd());

        //initialize the stack of rooms to be visited
        stack<Room*> toVisit;

        //add a random room as the start point and mark it as visited
        toVisit.push(rooms.at(random() % rooms.size()));
        toVisit.top()->visited = true;

        while (!toVisit.empty()) //while there are still rooms with potential unvisited neighbors
        {
            //get the next room
            Room* curr = toVisit.top();
            toVisit.pop();

            //check for unvisited neighbors
            vector<int> unvisitedNeighbors = UnvisitedNeighbors(curr->position);

            if (!unvisitedNeighbors.empty()) // if there are unvisited neighbors
            {
                toVisit.push(curr); //push the current room
                //choose a random unvisited neighbor
                Room* next = rooms.at(unvisitedNeighbors.at(random() % unvisitedNeighbors.size()));
                //add a passage between the current room and the next one
                passages.at(curr->position).emplace_back(next);
                passages.at(next->position).emplace_back(curr);
                //push the next room
                next->visited = true;
                toVisit.push(next);
            }
        }
    }

    vector<int> Dungeon_Map::UnvisitedNeighbors(const int curr) const
    {
        vector<int> unvisitedNeighbors; // a list of neighboring cells that have not been visited by RandomizedDFS

        //if there is a room bellow this one, and it has not been visited add it to the list
        if ((curr - width) >= 0 && !rooms.at(curr - width)->visited){
            unvisitedNeighbors.emplace_back(curr - width);
        }
        //if there is a room above this one, and it has not been visited add it to the list
        if ((curr + width) < rooms.size() && !rooms.at(curr + width)->visited){
            unvisitedNeighbors.emplace_back(curr + width);
        }
        //if there is a room to the left of this one, and it has not been visited add it to the list
        if ((curr % width) > 0 && !rooms.at(curr - 1)->visited){ //if
            unvisitedNeighbors.emplace_back(curr - 1);
        }
        //if there is a room to the right of this one, and it has not been visited add it to the list
        if ((curr % width) < (width - 1) && curr != rooms.size() - 1 && !rooms.at(curr + 1)->visited) {
            unvisitedNeighbors.emplace_back(curr + 1);
        }

        return unvisitedNeighbors; //return the list of unvisited neighbors
    }

    void Dungeon_Map::generateMazeSVG() const
    {
        auto mapFile = ofstream("Dungeon_Maze.svg"); //open the file to write to

        //Write the SVG header to the file
        mapFile << std::regex_replace(
            std::regex_replace(SVG_HEAD, std::regex("X"), std::to_string(width * 10)),
            std::regex("Y"),
            std::to_string(height * 10)) << std::endl;

        //define x and y values to track location on the resulting image
        int x = 0, y = 0;

        //for each room
        for (Room* room : rooms)
        {
            bool up = false, right = false, down = false, left = false;
            for (Room* neighbour : passages.at(room->position)){
                if (neighbour->position == room->position + width) {up = true;}
                else if (neighbour->position == room->position + 1) {right = true;}
                else if (neighbour->position == room->position - width) {down = true;}
                else if (neighbour->position == room->position - 1) {left = true;}
            }
            if (!up){
                mapFile << SVGLine(x, y + 10, x + 10, y + 10) << std::endl;
            }
            if (!right){
                mapFile << SVGLine(x + 10, y, x + 10, y + 10) << std::endl;
            }
            if (!down){
                mapFile << SVGLine(x, y, x + 10, y) << std::endl;
            }
            if (!left){
                mapFile << SVGLine(x, y, x, y + 10) << std::endl;
            }

            if (room->position % width == width - 1) {y += 10; x = 0;}
            else {x += 10;}
        }

        mapFile << SVG_FOOT;
    }

    std::string Dungeon_Map::SVGLine(const int x1, const int y1, const int x2, const int y2)
    {
        string lineSVG = regex_replace(SVG_LINE, regex("X1"), std::to_string(x1));
        lineSVG = regex_replace(lineSVG, regex("Y1"), std::to_string(y1));
        lineSVG = regex_replace(lineSVG, regex("X2"), std::to_string(x2));
        return regex_replace(lineSVG, regex("Y2"), std::to_string(y2));
    }

    std::string Dungeon_Map::SVGRoom(Room* room, const int xOffset, const int yOffset) {
        std::random_device rd;
        std::mt19937 random(rd());

        room->width = ROOM_SIZES[random() % std::size(ROOM_SIZES)];

        //if width is 10 the room is a passage extension and should have a height of 10
        if (room->width == 10) {
            room->height = 10;
        }
        else {
            room->height = ROOM_SIZES[random() % std::size(ROOM_SIZES)];
        }

        int minX = (CELL_SIZE / 2) - (PASSAGE_SIZE / 2) - (room->width - PASSAGE_SIZE);
        if (minX < 0) { minX = 0; };
        int minY = (CELL_SIZE / 2) - (PASSAGE_SIZE / 2) - (room->height - PASSAGE_SIZE);
        if (minY < 0) { minY = 0; };
        int maxX = CELL_SIZE - room->width;
        if (maxX > (CELL_SIZE / 2) - (PASSAGE_SIZE / 2)) { maxX = (CELL_SIZE / 2) - (PASSAGE_SIZE / 2); };
        int maxY = CELL_SIZE - room->height;
        if (maxY > (CELL_SIZE / 2) - (PASSAGE_SIZE / 2)) { maxY = (CELL_SIZE / 2) - (PASSAGE_SIZE / 2); };

        int roomX = static_cast<int>(random() % (maxX - minX + 1)) + minX;
        roomX -= roomX % 5;
        int roomY = static_cast<int>(random() % (maxY - minY + 1)) + minY;
        roomY -= roomY % 5;

        std::string roomSVG;
        const int tileX = 50 * (room->relPos.first - xOffset);
        const int tileY = 50 * (room->relPos.second - yOffset);

        if (std::ranges::find(room->exits, north) != room->exits.end()) {
            roomSVG.append(SVGLine(tileX + 20, tileY + roomY + room->height, tileX + 20, tileY + 50) + "\n");
            roomSVG.append(SVGLine(tileX + 30, tileY + roomY + room->height, tileX + 30, tileY + 50) + "\n");
            roomSVG.append(SVGLine(tileX + roomX, tileY + roomY + room->height, tileX + 20, tileY + roomY + room->height) + "\n");
            roomSVG.append(SVGLine(tileX + 30, tileY + roomY + room->height, tileX + roomX + room->width, tileY + roomY + room->height) + "\n");
        }
        else {
            roomSVG.append(SVGLine(tileX + roomX, tileY + roomY + room->height, tileX + roomX + room->width, tileY + roomY + room->height) + "\n");
        }

        if (std::ranges::find(room->exits, east) != room->exits.end()) {
            roomSVG.append(SVGLine(tileX + roomX + room->width, tileY + 20, tileX + 50, tileY + 20) + "\n");
            roomSVG.append(SVGLine(tileX + roomX + room->width, tileY + 30, tileX + 50, tileY + 30) + "\n");
            roomSVG.append(SVGLine(tileX + roomX + room->width, tileY + roomY, tileX + roomX + room->width, tileY + 20) + "\n");
            roomSVG.append(SVGLine(tileX + roomX + room->width, tileY + 30, tileX + roomX + room->width, tileY + roomY + room->height) + "\n");
        }
        else {
            roomSVG.append(SVGLine(tileX + roomX + room->width, tileY + roomY, tileX + roomX + room->width, tileY + roomY + room->height) + "\n");
        }

        if (std::ranges::find(room->exits, south) != room->exits.end()) {
            roomSVG.append(SVGLine(tileX + 20, tileY, tileX + 20, tileY + roomY) + "\n");
            roomSVG.append(SVGLine(tileX + 30, tileY, tileX + 30, tileY + roomY) + "\n");
            roomSVG.append(SVGLine(tileX + roomX, tileY + roomY, tileX + 20, tileY + roomY) + "\n");
            roomSVG.append(SVGLine(tileX + 30, tileY + roomY, tileX + roomX + room->width, tileY + roomY) + "\n");
        }
        else {
            roomSVG.append(SVGLine(tileX + roomX, tileY + roomY, tileX + roomX + room->width, tileY + roomY) + "\n");
        }

        if (std::ranges::find(room->exits, west) != room->exits.end()) {
            roomSVG.append(SVGLine(tileX, tileY + 20, tileX + roomX, tileY + 20) + "\n");
            roomSVG.append(SVGLine(tileX, tileY + 30, tileX + roomX, tileY + 30) + "\n");
            roomSVG.append(SVGLine(tileX + roomX, tileY + roomY, tileX + roomX, tileY + 20) + "\n");
            roomSVG.append(SVGLine(tileX + roomX, tileY + 30, tileX + roomX, tileY + roomY + room->height) + "\n");
        }
        else {
            roomSVG.append(SVGLine(tileX + roomX, tileY + roomY, tileX + roomX, tileY + roomY + room->height) + "\n");
        }

        room->SVGString = roomSVG;
        return roomSVG;
    }

    bool Dungeon_Map::spaceAvailable(const Room *currRoom, Room *nextRoom, const direction next) const {
        auto proposedSpace = currRoom->relPos;
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
                break;
            default:
                std::cout << "Error in Dungeon_Map::spaceAvailable(): Invalid direction" << std::endl;
                exit(1);
        }

        for (const Room* room : rooms) {
            if (room->relPos == proposedSpace){
                return false;
            }
        }
        return true;
    }

    void Dungeon_Map::placeRoom(Room *currRoom, Room *nextRoom, const direction next) {
        currRoom->exits.push_back(next);
        switch (next) {
            case north:
                nextRoom->exits.push_back(south);
                nextRoom->relPos = {currRoom->relPos.first, currRoom->relPos.second + 1};
                break;
            case east:
                nextRoom->exits.push_back(west);
                nextRoom->relPos = {currRoom->relPos.first + 1, currRoom->relPos.second};
                break;
            case south:
                nextRoom->exits.push_back(north);
                nextRoom->relPos = {currRoom->relPos.first, currRoom->relPos.second - 1};
                break;
            case west:
                nextRoom->exits.push_back(east);
                nextRoom->relPos = {currRoom->relPos.first - 1, currRoom->relPos.second};
                break;
            default:
                std::cout << "Error in Dungeon_Map::placeRoom(): Invalid direction" << std::endl;
                exit(1);
        }

    }

    std::string Dungeon_Map::describeExits(const Room *room) {
        bool first = true;
        std::string doorDescriptions;
        for (const direction exit: room->exits) {
            std::string exitDir;
            switch (exit) {
                case north:
                    exitDir = "north";
                    break;
                case east:
                    exitDir = "east";
                    break;
                case south:
                    exitDir = "south";
                    break;
                case west:
                    exitDir = "west";
                    break;
                default:
                    std::cout << "Error in Dungeon_Map::describeExits(): Invalid exit Direction" << std::endl;
            }
            std::string currExitDescription = std::regex_replace(DOOR_DESCRIPTION, regex("DT"), DOOR_TABLE.roll());
            if (!first) {
                doorDescriptions += ",";
            }
            else {
                first = false;
            }
            doorDescriptions += std::regex_replace(currExitDescription, regex("DIR"), exitDir);
        }
        if (doorDescriptions.empty()) {
            std::cout << "Error in Dungeon_Map::describeExits: room has no exits" << std::endl;
        }
        return doorDescriptions;
    }

    void Dungeon_Map::generateDungeonSVG() const {
        std::random_device rd;
        std::mt19937 random(rd());

        std::stack<Room*> toGenerate;
        rooms.at(0)->relPos = std::pair<int,int>(0, 0);
        toGenerate.push(rooms.at(0));

        while (!toGenerate.empty()) {
            Room* curroom = toGenerate.top();
            toGenerate.pop();

            if (!curroom->generated) {
                curroom->generated = true;
                for (Room* neighbour : passages.at(curroom->position)) {
                    if (!neighbour->generated) {
                        bool availableSpace = false;
                        while (!availableSpace) {
                            auto next = static_cast<direction>(random() % 4);
                            if (spaceAvailable(curroom, neighbour, next)) {
                                placeRoom(curroom, neighbour, next);
                                availableSpace = true;
                            }
                        }
                        toGenerate.push(neighbour);
                    }
                }
            }
        }

        int minX = 0, minY = 0, maxX = 0, maxY = 0;
        for (Room* room : rooms) {
            if (room->relPos.first > maxX) {
                maxX = room->relPos.first;
            }
            if (room->relPos.second > maxY) {
                maxY = room->relPos.second;
            }
            if (room->relPos.first < minX) {
                minX = room->relPos.first;
            }
            if (room->relPos.second < minY) {
                minY = room->relPos.second;
            }
        }

        int mapWidth = (maxX - minX) * CELL_SIZE;
        int mapHeight = (maxY - minY) * CELL_SIZE;

        ofstream mapFile = ofstream("Dungeon_Map.svg");
        string header = std::regex_replace(SVG_HEAD, std::regex("X"), std::to_string(mapWidth + CELL_SIZE));
        mapFile << std::regex_replace(header, std::regex("Y"), std::to_string(mapHeight + CELL_SIZE)) << std::endl;

        for (Room* room : rooms) {
            mapFile << SVGRoom(room, minX, minY) << std::endl;
        }

        mapFile << SVG_FOOT;
    }

    void Dungeon_Map::generateDungeonDescription() {
        //Determine Dungeon Type
        const std::string roomUseFile = "Table_Files/DungeonTypes/" + DUNGEON_TYPE_TABLE.roll() + ".txt";
        const auto ROOM_USE = Table::Random_Table(roomUseFile);

        for (Room* room : rooms) {
            room->description = std::regex_replace(ROOM_DESCRIPTION, regex("N"), std::to_string(room->exits.size()));
            room->description = std::regex_replace(room->description, regex("ED"), describeExits(room));
            room->description = std::regex_replace(room->description, regex("RU"), ROOM_USE.roll());
            room->description = std::regex_replace(room->description, regex("RC"), ROOM_CONDITION_TABLE.roll());
            std::cout << room->description << std::endl;
        }
    }

    Dungeon_Map::~Dungeon_Map() {
        for (Room* room : rooms) {
            delete room;
        }
    }
} // DungeonGenerator