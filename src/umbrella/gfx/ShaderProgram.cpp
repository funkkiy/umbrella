#include "gfx/ShaderProgram.h"

#include <glad/gl.h>
#include <optional>
#include <spdlog/spdlog.h>
#include <string>

namespace Umbrella::Gfx {

std::optional<GLuint> CompileProgram(
    std::string const& vsSource, std::string const& fsSource)
{
    GLint shadersOk = true;

    auto printErrorMsg = [](GLuint shader) -> void {
        GLchar error[512];
        GLsizei errorLen = 0;
        glGetShaderInfoLog(shader, 512, &errorLen, error);
        spdlog::error("{}", error);
    };

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    char const* vsSourceRaw = vsSource.c_str();
    glShaderSource(vertexShader, 1, &vsSourceRaw, nullptr);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &shadersOk);
    if (!shadersOk) {
        printErrorMsg(vertexShader);
        return {};
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    char const* fsSourceRaw = fsSource.c_str();
    glShaderSource(fragmentShader, 1, &fsSourceRaw, nullptr);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &shadersOk);
    if (!shadersOk) {
        printErrorMsg(fragmentShader);
        return {};
    }

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &shadersOk);
    if (!shadersOk) {
        return {};
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

} // namespace Umbrella::Gfx
