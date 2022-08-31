#pragma once

#include <string>
#include <vector>

struct GameName {
    std::string name;
    unsigned char number;
};

class Category {
public:
    Category(unsigned char categoryNumber, std::string categoryTitle);

    void addName(std::string nameString, const unsigned char gameNumber);
    std::vector<unsigned char> getStrings() const;
    std::vector<unsigned char> getLocations(unsigned int startAddress) const;

    std::vector<unsigned char> getCategoryHeader(unsigned int gameListOffset) const;

private:
    std::vector<GameName> games;
    
    unsigned char categoryNumber;
    std::vector<unsigned char> categoryTitleChars;
};

struct CategoryData {
    std::vector<unsigned char> headers;
    std::vector<unsigned char> gameListLocations;
    std::vector<unsigned char> gameNameLocations;
    std::vector<unsigned char> gameNameStrings;
};

class Categories {
public:
    Categories(const std::vector<std::string>& names);
    void addName(int categoryIndex, const std::string& name, const unsigned char gameNumber);
    CategoryData generate() const;
private:
    std::vector<Category> categories;
};