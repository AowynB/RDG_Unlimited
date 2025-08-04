//
// Created by Aowyn on 25/07/2025.
//

#ifndef RANDOM_TABLE_H
#define RANDOM_TABLE_H
#include <random>


class Random_Table {
private:
    std::string filename;
    std::vector<std::string>* values;
    std::vector<std::string*>* table;
public:
    Random_Table(const std::string &filename);
    ~Random_Table();
};



#endif //RANDOM_TABLE_H
