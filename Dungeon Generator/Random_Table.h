//
// Created by Aowyn on 07/08/2025.
//

#ifndef RANDOM_TABLE_H
#define RANDOM_TABLE_H
#include <random>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>

namespace Table {

class Random_Table {
private:
    static std::mt19937* random; //Sudo random number generator for "rolling" on the table
    std::vector<std::pair<int, std::string>> table; //data structure containing the table
    int diceMax; //the maximum number that can be rolled
public:
    /**
     * Constructor for the Random_Table class
     * Initializes and seeds the mt19937 random number generator
     * Reads the provided file line by line and saves all entries with their maximum roll and return value
     * saves diceMax as the highest maximum roll
     */
    explicit Random_Table(const std::string &fileName) {
        if (random == nullptr) {
            random = new std::mt19937(std::random_device()());
        }

        std::ifstream tableFile(fileName);
        if (!tableFile.is_open()) {
            std::cout << "Error opening file " << fileName << "\n";
            return;
        }

        while (!tableFile.eof()) {
            std::string line;
            std::pair<int, std::string> entry;

            std::getline(tableFile, line);
            entry.first = std::stoi(line.substr(0, line.find(' ')));
            entry.second = line.substr(line.find(' ') + 1);

            table.push_back(entry);
        }

        diceMax = table[table.size() - 1].first;
    }

    /**
     * roll begins by generating a random number
     * it then loops through the table vector to find the associated return value
     * and returns it
     * @return a randomly chosen return string from table
     */
    std::string Random_Table::roll() {
        const int index = static_cast<int>(random() % diceMax);
        for (const auto&[upper_bound, value] : table) {
            if (index < upper_bound) {
                return value;
            }
        }
        return "ERROR: invalid number generated";
    }
};
}; // Table

#endif //RANDOM_TABLE_H
