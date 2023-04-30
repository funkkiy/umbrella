#include <UmbrellaApplication.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <spdlog/spdlog.h>
#include <tiny_obj_loader.h>

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
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    constexpr int defaultWidth = 640;
    constexpr int defaultHeight = 480;
    m_window = glfwCreateWindow(defaultWidth, defaultHeight, "Umbrella", nullptr, nullptr);
    if (!m_window) {
        Stop();
        return InitializeResult::WindowCreationFail;
    }
    glfwMakeContextCurrent(m_window);

    m_windowWidth = defaultWidth;
    m_windowHeight = defaultHeight;

    // Resize the Viewport, in case the window size changes
    glfwSetWindowUserPointer(m_window, this);
    glfwSetWindowSizeCallback(m_window, [](GLFWwindow* window, int width, int height) {
        UmbrellaApplication* app = static_cast<UmbrellaApplication*>(glfwGetWindowUserPointer(window));
        app->m_windowWidth = width;
        app->m_windowHeight = height;
        glViewport(0, 0, width, height);
    });

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
    vertexSrc = Umbrella::Util::ReadFile("shaders/VertexShader.glsl");
    fragSrc = Umbrella::Util::ReadFile("shaders/FragShader.glsl");
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
        -0.5, -0.5, +0.0, +1.0, +0.0, +0.0, // Bottom Left Vertex
        +0.5, -0.5, +0.0, +0.0, +1.0, +0.0, // Bottom Right Vertex
        +0.0, +0.5, +0.0, +0.0, +0.0, +1.0  // Top Vertex
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

    // Pass MVP into Vertex Shader.
    GLuint projectionIdx = glGetUniformLocation(m_shaderProgram, "uProjection");
    GLuint viewIdx = glGetUniformLocation(m_shaderProgram, "uView");
    GLuint modelIdx = glGetUniformLocation(m_shaderProgram, "uModel");

    glm::mat4 model = glm::rotate(glm::mat4(1.0), glm::radians(45.0f), glm::vec3(0.0, 1.0, 0.0));
    glm::mat4 view = glm::translate(glm::mat4(1.0), glm::vec3(0.0, 0.0, -3.0));
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), static_cast<float>(m_windowWidth) / static_cast<float>(m_windowHeight), 0.1f, 100.0f);

    glUniformMatrix4fv(modelIdx, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewIdx, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionIdx, 1, GL_FALSE, glm::value_ptr(projection));

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
    spdlog::info("Stopping...");
    glfwTerminate();
}

void UmbrellaApplication::Run()
{
    spdlog::info("Started Umbrella.");

    if (Initialize() != InitializeResult::InitializeOk) {
        spdlog::error("Initialize() != InitializeResult::InitializeOk");
        return;
    };

    if (Prepare() != PrepareResult::PrepareOk) {
        spdlog::error("Prepare() != PrepareResult::PrepareOk");
        return;
    }

    while (!glfwWindowShouldClose(m_window)) {
        Tick();
    }

    Stop();
}

} // namespace Umbrella
