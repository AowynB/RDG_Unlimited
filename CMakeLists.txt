cmake_minimum_required(VERSION 3.30)
project(RDG_Unlimited)

set(CMAKE_CXX_STANDARD 26)

add_executable(RDG_Unlimited main.cpp
        "Dungeon Generator/Dungeon_Map.cpp"
        "Dungeon Generator/Dungeon_Map.h"
        main.cpp
)
