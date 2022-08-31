#pragma once

#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

class OutputFile {
public:
    OutputFile(fs::path path);
    void writeContent(std::vector<unsigned char> data);
    void replaceContent(std::vector<unsigned char> data, unsigned int address);
    unsigned int getBytesWritten();

private:
    fs::path path;
    int bytesWritten = 0;
};