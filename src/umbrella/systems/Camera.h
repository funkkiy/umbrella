#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <spdlog/spdlog.h>

namespace Umbrella {

struct Camera {
    Camera(glm::vec3 position, glm::vec3 focus)
        : m_position(position)
        , m_up(glm::vec3(0.0f, 1.0f, 0.0f))
        , m_right(glm::vec3(0.0f, 0.0f, 1.0f))
        , m_focus(focus)
    {
    }

    struct CameraInputState {
        bool w {false};
        bool s {false};
        bool a {false};
        bool d {false};
        bool q {false};
        bool e {false};
    } m_inputState;

    void ProcessKeys(int key, int action)
    {
        bool keyPressed = action != GLFW_RELEASE;

        switch (key) {
        case GLFW_KEY_W:
            m_inputState.w = keyPressed;
            break;
        case GLFW_KEY_S:
            m_inputState.s = keyPressed;
            break;
        case GLFW_KEY_A:
            m_inputState.a = keyPressed;
            break;
        case GLFW_KEY_D:
            m_inputState.d = keyPressed;
            break;
        case GLFW_KEY_Q:
            m_inputState.q = keyPressed;
            break;
        case GLFW_KEY_E:
            m_inputState.e = keyPressed;
            break;
        default:
            break;
        }
    }

    void Tick(float dt)
    {
        float camSpeed = 25.0f;
        glm::vec3 posDelta = glm::vec3(0.0f, 0.0f, 0.0f);
        if (m_inputState.w) {
            posDelta += glm::vec3(0.0f, 0.0f, -1.0f);
        }
        if (m_inputState.s) {
            posDelta += glm::vec3(0.0f, 0.0f, 1.0f);
        }
        if (m_inputState.a) {
            posDelta += glm::vec3(-1.0f, 0.0f, 0.0f);
        }
        if (m_inputState.d) {
            posDelta += glm::vec3(1.0f, 0.0f, 0.0f);
        }
        if (m_inputState.q) {
            posDelta += glm::vec3(0.0f, 1.0f, 0.0f);
        }
        if (m_inputState.e) {
            posDelta += glm::vec3(0.0f, -1.0f, 0.0f);
        }
        m_position += (dt * camSpeed * posDelta);
    }

    glm::vec3 m_position;
    glm::vec3 m_up;
    glm::vec3 m_right;
    glm::vec3 m_focus;
};

} // namespace Umbrella
