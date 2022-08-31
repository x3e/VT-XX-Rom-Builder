#pragma once

#include "header.h"

#include <vector>
#include <map>

const std::map<unsigned int, unsigned char> chrSizes {
    {512, 0},
    {256, 0},
    {128, 1},
    {64, 2},
    {32, 4},
    {16, 5},
    {8, 6},
    {4, 7},
    {0, 0}
};

const std::map<unsigned int, unsigned char> prgSizes {
    {512, 0},
    {256, 1},
    {128, 2},
    {64, 3},
    {32, 4},
    {16, 5},
    {8, 6},
    {4, 7},
    {0, 0}
};

const std::map<unsigned char, unsigned char> mappers {
    {0, 0},
    {1, 1},
    {2, 6},
    {3, 3},
    {4, 0},
    {5, 5}
};

struct Location {
    Location() {}
    Location(const Header& header) {
        prgSize = header.prgSize;
        chrSize = header.chrSize;
        verticalMirror = header.verticalMirror;
        mapper = header.mapper;
    }

    unsigned int prgStart = -1;
    unsigned int chrStart = -1;
    unsigned int prgSize = 0;
    unsigned int chrSize = 0;
    unsigned char mapper = 0;
    bool verticalMirror = false;

    std::vector<unsigned char> getDescriptor() const {
        std::vector<unsigned char> result;
        result.push_back(prgSizes.at(prgSize));
        result.push_back(0);
        result.push_back(chrSizes.at(chrSize));
        result.push_back(0);
        
        result.push_back(prgStart >> 2);
        result.push_back(prgStart >> 10);
        result.push_back(chrStart >> 2);
        result.push_back(chrStart >> 10);

        result.push_back(0);
        result.push_back(0);
        result.push_back(mappers.at(mapper));
        result.push_back(!verticalMirror);
        return result;
    }
};