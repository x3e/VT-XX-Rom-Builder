#include "outputfile.h"

#include <stdexcept>
#include <fstream>

OutputFile::OutputFile(fs::path path) : path(path) {
    fs::remove(path);
}

void OutputFile::writeContent(std::vector<unsigned char> data) {
    std::ofstream outputFile(path, std::ios::app);
    if (!outputFile.is_open())
        throw std::runtime_error("Could not open file: " + path.string());
    
    for (unsigned char dataByte : data) {
        outputFile.put(static_cast<char>(dataByte));
    }

    bytesWritten += data.size();

    outputFile.close();
}

void OutputFile::replaceContent(std::vector<unsigned char> data, unsigned int address) {
    std::fstream outputFile;
    outputFile.open(path, std::fstream::in | std::fstream::out);
    if (!outputFile.is_open())
        throw std::runtime_error("Could not open file: " + path.string());

    outputFile.seekp(address);
    
    for (unsigned char dataByte : data) {
        outputFile.put(static_cast<char>(dataByte));
    }

    outputFile.close();
}

unsigned int OutputFile::getBytesWritten() {
    return bytesWritten;
}

// std::vector<unsigned char> InputFile::getContent(const size_t start, const size_t length) {
//     std::vector<unsigned char> result;

//     std::ifstream input_file(path);
//     if (!input_file.is_open())
//         throw std::runtime_error("Could not open file: " + path.string());

//     char byte = 0;
//     size_t bytesRead = 0;
//     while (!input_file.eof() && bytesRead < length) {
//         input_file.get(byte);
//         result.push_back(static_cast<unsigned char>(byte));
//         bytesRead++;
//     }

//     input_file.close();
    
//     if (bytesRead != length)
//         throw std::runtime_error("Could not read from file: " + path.string());

//     return result;
// }