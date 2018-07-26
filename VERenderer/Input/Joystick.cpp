#include "stdafx.h"
#include "Joystick.h"


Joystick::Joystick(GLFWwindow * win)
{
}

Joystick::~Joystick()
{
}

std::vector<bool> Joystick::getButtonsStatus(int index)
{
    int count;
    const unsigned char* axes = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &count);
    auto res = std::vector<bool>();
    for (int i = 0; i < count; i++) res.push_back(axes[i] == GLFW_PRESS);
    return res;
}

bool Joystick::isPresent(int index)
{
    return glfwJoystickPresent(index) == GLFW_TRUE;
}

std::vector<float> Joystick::getAxes(int index)
{
    int count;
    const float* axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &count);
    auto res = std::vector<float>();
    for (int i = 0; i < count; i++) res.push_back(axes[i]);
    return res;
}
