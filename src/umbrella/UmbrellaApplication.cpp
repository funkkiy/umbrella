#include <UmbrellaApplication.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/gl.h>

#include "gfx/ShaderProgram.h"
#include "util/File.h"

namespace Umbrella {

InitializeResult UmbrellaApplication::Initialize()
{
    if (!glfwInit()) {
        return InitializeResult::GLFWInitFail;
    }

    // Create a window with a OpenGL 4.6 Core context
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    m_window = glfwCreateWindow(640, 480, "Umbrella", nullptr, nullptr);
    if (!m_window) {
        Stop();
        return InitializeResult::WindowCreationFail;
    }
    glfwMakeContextCurrent(m_window);

    if (!gladLoadGL(glfwGetProcAddress)) {
        Stop();
        return InitializeResult::GLADLoaderFail;
    }

    glfwSwapInterval(1);

    return InitializeResult::InitializeOk;
}

PrepareResult UmbrellaApplication::Prepare()
{
    std::optional<std::string> vertexSrc, fragSrc;
    vertexSrc = Umbrella::Util::ReadFile("VertexShader.glsl");
    fragSrc = Umbrella::Util::ReadFile("FragShader.glsl");
    if (!vertexSrc || !fragSrc) {
        return PrepareResult::SourceReadFail;
    }

    std::optional<GLuint> shaderProgram = Umbrella::Gfx::CompileProgram(*vertexSrc, *fragSrc);
    if (shaderProgram) {
        m_shaderProgram = *shaderProgram;
    } else {
        return PrepareResult::ShaderBuildFail;
    }

    float vtx[] = {
        -0.5, -0.5, +0.0, +1.0, +0.0, +0.0, // Vertex 1 (bottom left)
        +0.5, -0.5, +0.0, +0.0, +1.0, +0.0, // Vertex 2 (bottom right)
        +0.0, +0.5, +0.0, +0.0, +0.0, +1.0  // Vertex 3 (top)
    };

    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vtx), vtx, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    m_VAO = VAO;

    // Unbind VAO and VBO before use.
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return PrepareResult::PrepareOk;
}

void UmbrellaApplication::Render()
{
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glBindVertexArray(m_VAO);
    glUseProgram(m_shaderProgram);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void UmbrellaApplication::Tick()
{
    Render();

    glfwSwapBuffers(m_window);
    glfwPollEvents();
}

void UmbrellaApplication::Stop()
{
    glfwTerminate();
}

void UmbrellaApplication::Run()
{
    if (Initialize() != InitializeResult::InitializeOk || Prepare() != PrepareResult::PrepareOk) {
        return;
    };

    while (!glfwWindowShouldClose(m_window)) {
        Tick();
    }

    Stop();
}

} // namespace Umbrella
