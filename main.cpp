//
// Created by aowynbb on 09/06/25.
//

#include <iostream>

#include "Dungeon_Map/Dungeon_Map.h"

int main()
{
    std::random_device random;
    std::mt19937 generator(random());
    Dungeon_Map map(2000);
    map.generate_dungeon_layout(generator);
    map.generate_dungeon_svg(generator);
}
