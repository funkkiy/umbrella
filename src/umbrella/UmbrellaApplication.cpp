#include <UmbrellaApplication.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/gl.h>

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

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glfwSwapInterval(1);

    return true;
}

void UmbrellaApplication::Render()
{
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

    while (!glfwWindowShouldClose(m_window)) {
        Tick();
    }

    Stop();
}

} // namespace Umbrella