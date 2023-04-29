#pragma once

#include <cstdint>
#include <glad/gl.h>

struct GLFWwindow;

namespace Umbrella {

enum class InitializeResult : uint8_t {
    InitializeOk = 0,
    GLFWInitFail = 1,
    WindowCreationFail = 2,
    GLADLoaderFail = 3
};

enum class PrepareResult : uint8_t {
    PrepareOk = 0,
    SourceReadFail = 1,
    ShaderBuildFail = 2
};

class UmbrellaApplication {
public:
    void Run();

protected:
    InitializeResult Initialize();
    PrepareResult Prepare();
    void Tick();
    void Render();
    void Stop();

private:
    GLFWwindow* m_window;
    GLuint m_shaderProgram;
    GLuint m_VAO;
};

} // namespace Umbrella
