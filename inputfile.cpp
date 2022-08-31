#include "inputfile.h"

#include <stdexcept>
#include <fstream>

InputFile::InputFile(fs::path path) : path(path) {}

std::vector<unsigned char> InputFile::getContent(const size_t start, const size_t length) const {
    std::vector<unsigned char> result;

    std::ifstream input_file(path);
    if (!input_file.is_open())
        throw std::runtime_error("Could not open file: " + path.string());

    input_file.seekg(start, input_file.beg);

    char byte = 0;
    size_t bytesRead = 0;
    while (!input_file.eof() && bytesRead < length) {
        input_file.get(byte);
        result.push_back(static_cast<unsigned char>(byte));
        bytesRead++;
    }

    input_file.close();
    
    if (bytesRead != length)
        throw std::runtime_error("Could not read from file: " + path.string());

    return result;
}