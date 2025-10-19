//
// Created by aowynbb on 09/06/25.
//

#include <iostream>

#include "Dungeon_Map.h"
#include "old_dungeon_map/Dungeon_Map.h"

int main()
{
    Dungeon_Map map(10);
    map.generate_maze_svg();
}
