#pragma once

#include <optional>
#include <string>

namespace Umbrella::Util {

std::optional<std::string> ReadFile(char const* filePath);

} // namespace Umbrella::File
