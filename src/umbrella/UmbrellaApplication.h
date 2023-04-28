#pragma once

struct GLFWwindow;

namespace Umbrella {

class UmbrellaApplication {
public:
    void Run();

protected:
    bool Initialize();
    void Tick();
    void Render();
    void Stop();

private:
    GLFWwindow* m_window;
};

} // namespace Umbrella