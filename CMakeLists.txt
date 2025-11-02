cmake_minimum_required(VERSION 4.0)
project(RDG_Unlimited)

set(CMAKE_CXX_STANDARD 20)

add_executable(RDG_Unlimited main.cpp
        Helper_Classes_&_Files/Adjacency_List.h
        Dungeon_Map/Dungeon_Map.cpp
        Dungeon_Map/Dungeon_Map.h
        Helper_Classes_&_Files/SVG/SVG.cpp
        Helper_Classes_&_Files/SVG/SVG.h
)