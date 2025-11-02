//
// Created by aowyn on 10/30/25.
//

#ifndef RDG_UNLIMITED_SVG_H
#define RDG_UNLIMITED_SVG_H
#include <string>

std::string SVGHead(int x, int y);

std::string SVGEnd();

std::string SVGLine(int x1, int y1, int x2, int y2);

std::string SVG_corner(const std::pair<int, int> &start, const std::pair<int, int> &end, const std::pair<int, int> &turn);

#endif //RDG_UNLIMITED_SVG_H