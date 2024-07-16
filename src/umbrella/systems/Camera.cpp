#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Umbrella {

glm::vec3 DirectionFromEuler(double pitch, double yaw)
{
    float pitchRad = glm::radians(pitch);
    float yawRad = glm::radians(yaw);
    float cosPitch = std::cos(pitchRad);
    float sinPitch = std::sin(pitchRad);
    float cosYaw = std::cos(yawRad);
    float sinYaw = std::sin(yawRad);

    // Obtain the Direction
    return glm::normalize(
        glm::vec3(cosYaw * cosPitch, sinPitch, sinYaw * cosPitch));
}

Camera::Camera(glm::vec3 position)
    : m_position(position)
    , m_up(glm::vec3(0.0f, 1.0f, 0.0f))
{
    m_pitch = 0.0f;
    m_yaw = -89.0f;
    m_direction = DirectionFromEuler(m_pitch, m_yaw);
}

void Camera::ProcessKeys(int key, int action)
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

void Camera::ProcessMouse(double x, double y)
{
    if (firstMove) {
        m_inputState.prevX = x;
        m_inputState.prevY = y;
        firstMove = false;
    } else {
        m_inputState.prevX = m_inputState.x;
        m_inputState.prevY = m_inputState.y;
    }
    m_inputState.x = x;
    m_inputState.y = y;

    // Handle mouse movement
    float deltaX = m_inputState.x - m_inputState.prevX;
    float deltaY = m_inputState.prevY - m_inputState.y;

    constexpr float mouseSens = 0.1f;
    m_pitch += mouseSens * deltaY;
    m_yaw += mouseSens * deltaX;

    // Clamp Pitch to [-89.0f, 89.0f]
    m_pitch = std::clamp(m_pitch, -89.0f, 89.0f);
    // Clamp Yaw to [-360.0f, 360.0f]
    m_yaw = std::fmod(m_yaw, 360.0f);

    m_direction = DirectionFromEuler(m_pitch, m_yaw);
}

void Camera::Tick(float dt)
{
    constexpr float camSpeed = 25.0f;

    // Handle keyboard movement
    glm::vec3 posDelta = glm::vec3(0.0f, 0.0f, 0.0f);
    if (m_inputState.w) {
        posDelta += m_direction;
    }
    if (m_inputState.s) {
        posDelta -= m_direction;
    }
    if (m_inputState.a) {
        posDelta -= glm::normalize(glm::cross(m_direction, m_up));
    }
    if (m_inputState.d) {
        posDelta += glm::normalize(glm::cross(m_direction, m_up));
    }
    if (m_inputState.q) {
        posDelta -= m_up;
    }
    if (m_inputState.e) {
        posDelta += m_up;
    }
    m_position += (dt * camSpeed * posDelta);
}

} // namespace Umbrella
