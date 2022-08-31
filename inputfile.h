#pragma once

#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

class InputFile {
public:
    InputFile(fs::path path);
    std::vector<unsigned char> getContent(const size_t start, const size_t length) const;

protected:
    fs::path path;
};