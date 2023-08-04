#pragma once

#include <cstdint>
#include <glad/gl.h>

struct GLFWwindow;

namespace Umbrella {

enum class [[nodiscard]] InitializeResult : uint8_t {
    InitializeOk = 0,
    GLFWInitFail = 1,
    WindowCreationFail = 2,
    GLADLoaderFail = 3
};

enum class [[nodiscard]] PrepareResult : uint8_t {
    PrepareOk = 0,
    SourceReadFail = 1,
    ShaderBuildFail = 2,
    ObjLoadFail = 3,
    ObjParseFail = 4,
    TexLoadFail = 5
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
    GLFWwindow* m_window = nullptr;
    int m_windowWidth = 0;
    int m_windowHeight = 0;

    GLuint m_shaderProgram = 0;
    GLuint m_VAO = 0;
    GLuint m_meshTexture = 0;

    size_t m_numVertices = 0;
};

} // namespace Umbrella
