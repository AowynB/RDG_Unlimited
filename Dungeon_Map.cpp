//
// Created by aowyn on 10/19/25.
//
#include "Dungeon_Map.h"

#define SVG_HEAD R"(<svg width="X" height="Y" xmlns="http://www.w3.org/2000/svg">)"
#define SVG_LINE R"(<line x1="X1" y1="Y1" x2="X2" y2="Y2" style="stroke:white;stroke-width:2" />)"
#define SVG_RECT R"(<rect width="W" height="H" x="X" y="Y" style="stroke:white;stroke-width:2"/>)"
#define SVG_FOOT R"(</svg>)"

#include <fstream>
#include <regex>
#include "Adjacency_List.h"

std::string Dungeon_Map::SVGLine(const int x1, const int y1, const int x2, const int y2) {
    std::string lineSVG = regex_replace(SVG_LINE, std::regex("X1"), std::to_string(x1));
    lineSVG = regex_replace(lineSVG, std::regex("Y1"), std::to_string(y1));
    lineSVG = regex_replace(lineSVG, std::regex("X2"), std::to_string(x2));
    return regex_replace(lineSVG, std::regex("Y2"), std::to_string(y2));
}

Dungeon_Map::Dungeon_Map(const int size) {
    map_tiles = Adjacency_List<tile>();

    for (int i = 0; i < size; i++) {
        auto temp = tile(0, 0, i);
        map_tiles.add_vertex(temp);
    }
    std::random_device rand;
    std::mt19937 random_number_generator(rand());
    map_tiles.randomized_depth_first_search(random_number_generator);
}

void Dungeon_Map::generate_maze_svg()
{
    auto mapFile = std::ofstream("Dungeon_Maze.svg"); //open the file to write to
    int width = floor(sqrt(map_tiles.get_size()));
    int height = static_cast<int>(ceil(sqrt(map_tiles.get_size())) + 1);

    //Write the SVG header to the file
    mapFile << std::regex_replace(
        std::regex_replace(SVG_HEAD, std::regex("X"), std::to_string(width * 10)),
                            std::regex("Y"), std::to_string(height * 10)) << std::endl;

    //define x and y values to track location on the resulting image
    int x = 0, y = 0;

    //for each room
    for (int i = 0; i < map_tiles.get_size(); i++)
    {
        tile curr = map_tiles.get_vertex(i);
        bool up = false, right = false, down = false, left = false;
        for (auto neighbour : map_tiles.get_edges(curr.index)){
            if (neighbour == curr.index + width) {up = true;}
            else if (neighbour == curr.index + 1) {right = true;}
            else if (neighbour == curr.index - width) {down = true;}
            else if (neighbour == curr.index - 1) {left = true;}
        }
        if (!up){ mapFile << SVGLine(x, y + 10, x + 10, y + 10) << std::endl; }
        if (!right){ mapFile << SVGLine(x + 10, y, x + 10, y + 10) << std::endl; }
        if (!down){ mapFile << SVGLine(x, y, x + 10, y) << std::endl; }
        if (!left){ mapFile << SVGLine(x, y, x, y + 10) << std::endl; }

        if (curr.index % width == width - 1) {y += 10; x = 0;}
        else {x += 10;}
    }

    mapFile << SVG_FOOT;
}
