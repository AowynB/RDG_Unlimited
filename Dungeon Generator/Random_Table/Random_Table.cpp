//
// Created by Aowyn on 25/07/2025.
//

#include "Random_Table.h"

#include <fstream>
#include <iostream>
#include <regex>

const std::regex RANGEREGEX("[0-9]+-*[0-9]* ");

Random_Table::Random_Table(const std::string &filename)
{

    this->filename = filename;
    this->values = new std::vector<std::string>();
    this->table = new std::vector<std::string *>;

    std::ifstream file;
    file = std::ifstream(filename);
    if(!file.is_open()){
        std::cout<<"File Not Found"<<std::endl;
        return;
    }
    while (!file.eof()) {
        std::string line;
        std::getline(file, line);

        std::smatch range;
        std::regex_search(line, range, RANGEREGEX);
        std::string rangeStr = range.str();

        std::string lowerBound;
        std::string upperBound;
        if (rangeStr.find('-') != std::string::npos) {
            lowerBound = rangeStr.substr(0, rangeStr.find('-'));
            upperBound = rangeStr.substr(rangeStr.find('-') + 1, rangeStr.find(' '));
        }
        else {
            lowerBound = upperBound = rangeStr.substr(0, rangeStr.find(' '));
        }

        values->push_back(line.substr(line.find(' ') + 1));
        table->push_back(&
            values->at(values->size() - 1));
        if (lowerBound != upperBound) {
            int numinstances = std::stoi(upperBound) - std::stoi(lowerBound);
            for (int i = 1; i < numinstances; i++) {
                table->push_back(&values->at(values->size() - 1));
            }
        }
    }
}

Random_Table::~Random_Table() {
    delete this->values;
    delete this->table;
}
