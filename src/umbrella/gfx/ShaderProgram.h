#pragma once

#include <glad/gl.h>
#include <optional>
#include <string>

namespace Umbrella::Gfx {

std::optional<GLuint> CompileProgram(
    std::string const& vsSource, std::string const& fsSource);

} // namespace Umbrella::Gfx
