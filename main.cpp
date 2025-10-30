//
// Created by aowynbb on 09/06/25.
//

#include <iostream>

#include "Dungeon_Map.h"
#include "old_dungeon_map/Dungeon_Map.h"

int main()
{
    std::random_device random;
    std::mt19937 generator(random());
    Dungeon_Map map(100);
    map.generate_dungeon_layout(generator);
    map.generate_dungeon_svg();
}
