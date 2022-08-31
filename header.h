#pragma once

#include <vector>

class Header {
public:
    Header(std::vector<unsigned char> data);

    bool verticalMirror;
    unsigned int prgSize; // in kb
    unsigned int chrSize; // in kb
    unsigned char mapper;
};