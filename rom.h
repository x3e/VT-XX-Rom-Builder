#pragma once

#include "inputfile.h"
#include "header.h"

class Rom : public InputFile {
public:
    Rom (fs::path romPath);
    Header getHeader() const;

    std::vector<unsigned char> getPrgRom() const;
    std::vector<unsigned char> getChrRom() const;
    std::string getFileName() const;
    std::string getGameName() const;
    unsigned int getCategory() const;

    unsigned char index;

private:
    Header header;
};