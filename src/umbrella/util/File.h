#pragma once

#include <string>
#include <optional>

namespace Umbrella::Util {

std::optional<std::string> ReadFile(char const* filePath);

} // namespace Umbrella::File
