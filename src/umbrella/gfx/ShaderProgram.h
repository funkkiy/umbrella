#pragma once

#include <string>
#include <optional>
#include <glad/gl.h>

namespace Umbrella::Gfx {

std::optional<GLuint> CompileProgram(std::string const& vsSource, std::string const& fsSource);

} // namespace Umbrella::Gfx
