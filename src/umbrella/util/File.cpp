#include "util/File.h"

#include <fstream>
#include <optional>
#include <string>

namespace Umbrella::Util {

std::optional<std::string> ReadFile(char const* filePath)
{
    std::ifstream inputStream = std::ifstream(filePath, std::ios::in);
    if (inputStream) {
        std::string inputContents = {
            std::istreambuf_iterator<char>(inputStream),
            std::istreambuf_iterator<char>()};

        // If present, remove the last empty line
        size_t lastCharacterIdx = inputContents.length() - 1;
        if (inputContents[lastCharacterIdx] == '\n') {
            inputContents = inputContents.substr(0, lastCharacterIdx);
        }

        return inputContents;
    }

    return {};
}

} // namespace Umbrella::File
