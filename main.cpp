//
// Created by aowynbb on 09/06/25.
//

#include <iostream>

#include "Dungeon_Map.h"

int main()
{
    int numRooms;
    //std::cout << "Hello user, please enter the number of rooms for your dungeon: ";
    //std::cin >> numRooms;
    DungeonGenerator::Dungeon_Map dmap(500);
    dmap.RandomizedDFS();
    dmap.generateMazeSVG();
    dmap.generateDungeonSVG();
    //dmap.generateDungeonDescription();
}
