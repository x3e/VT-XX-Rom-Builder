#include <iostream>
#include <filesystem>
#include <stdexcept>
#include <map>
#include <functional>

#include "rom.h"
#include "header.h"
#include "outputfile.h"
#include "location.h"
#include "category.h"

namespace fs = std::filesystem;

fs::path dumpFilePath = "./dump/rom.nes";
fs::path romsPath = "./roms";
fs::path outputFilePath = "./output/output.nes";

std::map<std::string, Location> locations;

std::vector<Rom> romList;

OutputFile outputFile {outputFilePath};
InputFile dumpFile {dumpFilePath};

const unsigned int headerSize = 16;
const unsigned int gameListIndexesLocation = 0x681A0;
const unsigned int categoryHeaderLocation = 0x681D0;
const unsigned int numberOfGamesLocation = 0x681DD;
const unsigned int categoryTitleLocation = 0x681D4;
const unsigned int gameNameIndexesLocation = 0x682BF;
const unsigned int gameNameLocation = 0x6868F;
const unsigned int locationListLocation = 0x6C1F0;

const unsigned int menuChrStart = 0x46010;
const unsigned int menuChrEnd = 0x4E610;
const unsigned int menuRomStart = 0x63410;
const unsigned int menuRomEnd = 0x80010;

const unsigned int dumpSize = 0x0800010;


void copyUntil(const unsigned int addr) {
    if (addr < outputFile.getBytesWritten())
        throw std::runtime_error("previous section was too large to fit");
    outputFile.writeContent(dumpFile.getContent(outputFile.getBytesWritten(), addr - outputFile.getBytesWritten()));
}

bool romOverwritesAddress(unsigned int romStart, unsigned int romSize, unsigned int address) {
    return romStart <= address && romStart + romSize > address;
}

void writeRomsUntilAddress(unsigned int);

void writeRom(const Rom& rom, bool chr, unsigned int maxAddress) {
    unsigned int size = (chr ? rom.getHeader().chrSize : rom.getHeader().prgSize) * 1024;
    Location& loc = locations.at(rom.getGameName());
    unsigned int &romLocation = chr ? loc.chrStart : loc.prgStart;

    if (size == 0) return;
    if (romLocation != -1) return;

    unsigned int alignedAddress = ((outputFile.getBytesWritten() - 17) / size + 1) * size + 16;
    if (alignedAddress + size > maxAddress) return;
    
    std::vector<unsigned char> data = chr ? rom.getChrRom() : rom.getPrgRom();

    if (alignedAddress > outputFile.getBytesWritten()) {
        writeRomsUntilAddress(alignedAddress);
        unsigned int spaceWasted = alignedAddress - outputFile.getBytesWritten();
        if (spaceWasted > 0) std::cout << "space wasted: " << spaceWasted << " bytes" << std::endl;
    }
    copyUntil(alignedAddress);
    std::cout << "writing " << (chr?"CHR":"PRG") << "-ROM (" << rom.getGameName() << ")" << std::endl;
    
    romLocation = outputFile.getBytesWritten() / 1024;
    outputFile.writeContent(data);
}

void writeRomsUntilAddress(unsigned int maxAddress) {
    std::sort(romList.begin(), romList.end(), [](const Rom &a, const Rom &b) -> bool {
        return a.getHeader().prgSize > b.getHeader().prgSize;
    });
    for (const Rom& rom : romList) {
        writeRom(rom, false, maxAddress);
    }
    std::sort(romList.begin(), romList.end(), [](const Rom &a, const Rom &b) -> bool {
        return a.getHeader().chrSize > b.getHeader().chrSize;
    });
    for (const Rom& rom : romList) {
        writeRom(rom, true, maxAddress);
    }
    unsigned int spaceWasted = maxAddress - outputFile.getBytesWritten();
    if (spaceWasted > 0) std::cout << "space wasted: " << spaceWasted << " bytes" << std::endl;
}

int main() {
    if (!fs::is_directory(romsPath))
        throw std::runtime_error("rom path is not a directory");
    for (const auto & entry : fs::directory_iterator(romsPath)) {
        if (entry.is_directory()) {
            for (const auto & subEntry : fs::directory_iterator(entry)) {
                Rom rom {subEntry};
                romList.push_back(rom);
                std::string name = rom.getGameName();
                locations[name] = Location(rom.getHeader());
            }
        }
    }

    Categories categories {{"ACTION", "FIGHT", "SPORT", "SHOOT", "RACING", "PUZZLE"}};
    int gameIndex = 0;
    for (Rom& rom : romList) {
        rom.index = gameIndex;
        categories.addName(rom.getCategory(), rom.getGameName(), gameIndex);
        gameIndex++;
    }

    copyUntil(headerSize);
    writeRomsUntilAddress(menuChrStart);
    std::cout << "writing menu chr" << std::endl;
    copyUntil(menuChrEnd);
    writeRomsUntilAddress(menuRomStart);
    std::cout << "writing menu prg" << std::endl;
    copyUntil(menuRomEnd);
    writeRomsUntilAddress(dumpSize);

    std::cout << "ROMS take up " << (outputFile.getBytesWritten()/1024) << " KiB" << std::endl;

    copyUntil(dumpSize);
    
    CategoryData categoryData = categories.generate();
    outputFile.replaceContent(categoryData.headers, categoryHeaderLocation);
    outputFile.replaceContent(categoryData.gameListLocations, gameListIndexesLocation);
    outputFile.replaceContent(categoryData.gameNameLocations, gameNameIndexesLocation);
    outputFile.replaceContent(categoryData.gameNameStrings, gameNameLocation);
    
    std::sort(romList.begin(), romList.end(), [](const Rom &a, const Rom &b) -> bool {
        return a.index < b.index;
    });

    std::vector<unsigned char> descriptors;
    for (const Rom& rom : romList) {
        std::string name = rom.getGameName();
        Location& location = locations.at(name);
        if (location.prgStart == -1 || location.chrStart == -1) {
            std::cout << name << std::endl;
            std::cout << "prg rom size: " << location.prgSize << " KiB" << std::endl;
            std::cout << "chr rom size: " << location.chrSize << " KiB" << std::endl;
            std::cout << "prg rom start: " << location.prgStart << std::endl;
            std::cout << "chr rom start: " << location.chrStart << std::endl;
            std::cout << "mirroring: " << (location.verticalMirror ? "vertical":"horizontal") << std::endl;
            std::cout << "mapper: " << std::to_string(location.mapper) << std::endl;
            std::cout << std::endl;
            throw std::runtime_error("couldn't fit all roms");
        }
        std::vector<unsigned char> descriptor = location.getDescriptor();
        descriptors.insert(descriptors.end(), descriptor.begin(), descriptor.end());
    }

    outputFile.replaceContent(descriptors, locationListLocation);

    return 0;
}