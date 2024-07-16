#pragma once

#include <cstdint>
#include <memory>
#include <glad/gl.h>

#include "systems/Camera.h"

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
    ObjMissingAttrib = 5,
    TexLoadFail = 6
};

class UmbrellaApplication {
public:
    void Run();

protected:
    InitializeResult Initialize();
    PrepareResult Prepare();
    void Tick(float dt);
    void Render();
    void Stop();

    static void ProcessKeys(
        GLFWwindow* window, int key, int scancode, int action, int mods);
    static void ProcessMouse(GLFWwindow* window, double xpos, double ypos);

private:
    GLFWwindow* m_window {};
    int m_windowWidth {};
    int m_windowHeight {};

    GLuint m_shaderProgram {};
    GLuint m_VAO {};
    GLuint m_meshTexture {};
    size_t m_numVertices {};

    double m_lastTick {};

    std::unique_ptr<Camera> m_currentCamera = std::make_unique<Camera>(glm::vec3(0.0f, 0.0f, 7.0f));
};

} // namespace Umbrella
