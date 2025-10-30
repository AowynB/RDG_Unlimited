//
// Created by aowyn on 10/30/25.
//

#include "SVG.h"
#include <regex>

#define SVG_LINE R"(<line x1="X1" y1="Y1" x2="X2" y2="Y2" style="stroke:white;stroke-width:2" />)"
#define SVG_RECT R"(<rect width="W" height="H" x="X" y="Y" style="stroke:white;stroke-width:2"/>)"

std::string SVGHead(const int x, const int y) {
    return R"(<svg width=")" + std::to_string(x) + R"(" height=")" + std::to_string(y) + R"(" xmlns="http://www.w3.org/2000/svg">)";
}

std::string SVGEnd() {
    return R"(</svg>)";
}

std::string SVGLine(const int x1, const int y1, const int x2, const int y2) {
    std::string lineSVG = regex_replace(SVG_LINE, std::regex("X1"), std::to_string(x1));
    lineSVG = regex_replace(lineSVG, std::regex("Y1"), std::to_string(y1));
    lineSVG = regex_replace(lineSVG, std::regex("X2"), std::to_string(x2));
    return regex_replace(lineSVG, std::regex("Y2"), std::to_string(y2));
}

std::string SVG_corner(const std::pair<int, int> &start, const std::pair<int, int> &corner, const std::pair<int, int> &end) {
    std::string result;
    result += SVGLine(start.first, start.second, corner.first, corner.second);
    result += SVGLine(corner.first, corner.second, end.first, end.second);
    return result;
}