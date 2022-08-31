#include "header.h"

Header::Header(std::vector<unsigned char> data) {
    verticalMirror = data.at(6) & 0b1;
    prgSize = static_cast<unsigned int>(data.at(4)) * 16;
    chrSize = static_cast<unsigned int>(data.at(5)) * 8;
    mapper = (data.at(6) & 0b11110000) >> 4;
    mapper |= (data.at(7) & 0b11110000);
}