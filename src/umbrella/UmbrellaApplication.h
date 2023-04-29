#pragma once

#include <glad/gl.h>

struct GLFWwindow;

namespace Umbrella {

class UmbrellaApplication {
public:
    void Run();

protected:
    bool Initialize();
    bool Prepare();
    void Tick();
    void Render();
    void Stop();

private:
    GLFWwindow* m_window;
    GLuint m_shaderProgram;
};

} // namespace Umbrella
