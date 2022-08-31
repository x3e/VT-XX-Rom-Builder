#include "rom.h"
#include "header.h"

#include <string>

Rom::Rom(fs::path romPath) : InputFile(romPath), header(getContent(0, 16)) {}

Header Rom::getHeader() const {
    return header;
}

std::vector<unsigned char> Rom::getPrgRom() const {
    return getContent(16, header.prgSize*1024);
}

std::vector<unsigned char> Rom::getChrRom() const {
    return getContent(16 + header.prgSize*1024, header.chrSize*1024);
}

std::string Rom::getFileName() const {
    return path.filename();
}

std::string Rom::getGameName() const {
    std::string filename = path.filename();
    if (filename.find('(') != std::string::npos)
        return filename.substr(0, filename.find('('));
    if (filename.find('.') != std::string::npos)
        return filename.substr(0, filename.find('.'));
    return filename;
}

unsigned int Rom::getCategory() const {
    std::string subfolder = path.parent_path().filename();
    char firstLetter = subfolder.at(0);
    if (firstLetter >= '0' && firstLetter <= '5')
        return firstLetter - '0';
    return 0;
}