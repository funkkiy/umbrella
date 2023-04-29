#include <UmbrellaApplication.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/gl.h>

#include "gfx/ShaderProgram.h"
#include "util/File.h"

namespace Umbrella {

bool UmbrellaApplication::Initialize()
{
    if (!glfwInit()) {
        return false;
    }

    // Create a window with a OpenGL 4.6 Core context
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    m_window = glfwCreateWindow(640, 480, "Umbrella", nullptr, nullptr);
    if (!m_window) {
        Stop();
        return false;
    }
    glfwMakeContextCurrent(m_window);

    if (!gladLoadGL(glfwGetProcAddress)) {
        Stop();
        return false;
    }

    glfwSwapInterval(1);

    return true;
}

bool UmbrellaApplication::Prepare()
{
    std::optional<std::string> vertexSrc, fragSrc;
    vertexSrc = Umbrella::Util::ReadFile("VertexShader.glsl");
    fragSrc = Umbrella::Util::ReadFile("FragShader.glsl");
    if (!vertexSrc || !fragSrc) {
        // Failed to read shader sources
        return false;
    }

    std::optional<GLuint> shaderProgram = Umbrella::Gfx::CompileProgram(*vertexSrc, *fragSrc);
    if (shaderProgram) {
        m_shaderProgram = *shaderProgram;
    } else {
        // Failed to build shaders
        return false;
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

    // glBindBuffer(GL_ARRAY_BUFFER, 0);
    // glBindVertexArray(0);

    return true;
}

void UmbrellaApplication::Render()
{
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);

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
    bool appInitOk = Initialize();
    if (!appInitOk) {
        return;
    };

    Prepare();
    while (!glfwWindowShouldClose(m_window)) {
        Tick();
    }

    Stop();
}

} // namespace Umbrella
