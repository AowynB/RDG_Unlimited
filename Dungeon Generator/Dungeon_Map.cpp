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
#define SVG_FOOT R"(</svg>)"

using std::stack, std::string, std::ofstream;

namespace DungeonGenerator {
    Dungeon_Map::Dungeon_Map(int size)
    {
        rooms = new vector<Room*>;
        passages = new vector<vector<Room*>*>;
        for (int i = 0; i < size; i++)
        {
            rooms->emplace_back(new Room(false, i));
            passages->emplace_back(new vector<Room*>());
        }

        width = floor(sqrt(size));
        height = ceil(sqrt(size)) + 1;
        RandomizedDFS();
        generateSVG();
    }

    void Dungeon_Map::RandomizedDFS() const
    {
        //initialize the random number generator
        std::random_device random;

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
                rooms->at(next->position)->visited = true;
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

    void Dungeon_Map::generateSVG() const
    {
        ofstream mapFile = ofstream("Dungeon_Map.svg");
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

    std::string Dungeon_Map::SVGLine(int x1, int y1, int x2, int y2) const
    {
        string lineSVG = std::regex_replace(SVG_LINE, std::regex("X1"), std::to_string(x1));
        lineSVG = std::regex_replace(lineSVG, std::regex("Y1"), std::to_string(y1));
        lineSVG = std::regex_replace(lineSVG, std::regex("X2"), std::to_string(x2));
        return std::regex_replace(lineSVG, std::regex("Y2"), std::to_string(y2));
    }
} // DungeonGenerator