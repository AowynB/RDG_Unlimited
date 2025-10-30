cmake_minimum_required(VERSION 4.0)
project(RDG_Unlimited)

set(CMAKE_CXX_STANDARD 20)

add_executable(RDG_Unlimited main.cpp
        Adjacency_List.h
        Dungeon_Map.cpp
        Dungeon_Map.h
        SVG.cpp
        SVG.h
)