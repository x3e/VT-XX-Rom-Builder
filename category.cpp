#include "category.h"

#include <algorithm>

const unsigned int nameStringsAddress = 0x6867F;
const unsigned int namePointersAddress = 0x682AF;

Category::Category(unsigned char categoryNumber, std::string categoryTitle) :
    categoryNumber(categoryNumber) {
        std::transform(categoryTitle.begin(), categoryTitle.end(),categoryTitle.begin(), ::toupper);
        categoryTitleChars.insert(categoryTitleChars.end(), categoryTitle.begin(), categoryTitle.end());
        std::vector<unsigned char> emptychars = {0,0,0,0,0,0,0};
        categoryTitleChars.insert(categoryTitleChars.end(), emptychars.begin(), emptychars.end());
        categoryTitleChars.resize(7);
        if (categoryNumber != 0)
            categoryTitleChars.push_back(' ');
        categoryTitleChars.push_back(0xFF);
    }

void Category::addName(std::string nameString, const unsigned char gameNumber) {
    std::transform(nameString.begin(), nameString.end(),nameString.begin(), ::toupper);
    nameString += "\xFF";
    games.push_back({nameString, gameNumber});
}

std::vector<unsigned char> Category::getStrings() const {
    std::vector<unsigned char> result;
    for (const GameName& game : games)
        result.insert(result.end(), game.name.begin(), game.name.end());
    return result;
}
std::vector<unsigned char> Category::getLocations(unsigned int startAddress) const {
    unsigned int address = startAddress;
    std::vector<unsigned char> result;
    unsigned char nameNumber = 0;
    for (const GameName& game : games) {
        result.push_back(address);
        result.push_back(address >> 8);
        result.push_back(game.number);
        result.push_back(0);
        address += game.name.size();
    }
    return result;
}

std::vector<unsigned char> Category::getCategoryHeader(unsigned int gameListOffset) const {
    std::vector<unsigned char> result;
    result.push_back(0x05); //horizontal position of title
    result.push_back(0x03); //vertical position of title
    result.push_back(0x07); //string length of title
    result.push_back(0x01);
    result.push_back(0x00);
    result.insert(result.end(), categoryTitleChars.begin(), categoryTitleChars.end());
    result.push_back(games.size());
    result.push_back(0x00);
    result.push_back(0x00);
    result.push_back(0x00);
    result.push_back(0x09); //number of games per screen
    result.push_back(0x01); //number of lines per game
    result.push_back(0x05); //horizontal position of game list
    result.push_back(0x06); //vertical position of game list
    result.push_back(gameListOffset);
    result.push_back(gameListOffset >> 8);
    result.push_back(0x8C); //doesn't seem important
    result.push_back(0x81); //doesn't seem important
    result.push_back(0x8E); //doesn't seem important
    result.push_back(0x81); //seems to be used for something
    result.push_back(games.size());
    result.push_back(0x00);
    return result;
}

Categories::Categories(const std::vector<std::string>& names) {
    unsigned char index = 0;
    for(const std::string& name : names) categories.push_back(Category {index++, name});
}

CategoryData Categories::generate() const {
    CategoryData result;
    for (const Category& category : categories) {
        unsigned int gameListOffset = namePointersAddress + result.gameNameLocations.size();
        std::vector<unsigned char> nameStrings = category.getStrings();
        std::vector<unsigned char> locations = category.getLocations(nameStringsAddress + result.gameNameStrings.size());
        std::vector<unsigned char> header = category.getCategoryHeader(gameListOffset);
        result.gameNameStrings.insert(result.gameNameStrings.end(), nameStrings.begin(), nameStrings.end());
        result.gameNameLocations.insert(result.gameNameLocations.end(), locations.begin(), locations.end());
        result.gameNameLocations.push_back(0xFF);
        result.gameNameLocations.push_back(0xFF);
        result.gameListLocations.push_back(gameListOffset);
        result.gameListLocations.push_back(gameListOffset >> 8);
        result.headers.insert(result.headers.end(), header.begin(), header.end());
    }
    return result;
}

void Categories::addName(int categoryIndex, const std::string& name, const unsigned char gameNumber) {
    categories.at(categoryIndex).addName(name, gameNumber);
}