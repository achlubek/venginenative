#include "stdafx.h"
#include "Mouse.h"

Mouse * Mouse::instance = nullptr;

Mouse::Mouse(GLFWwindow* win)
{
    window = win;
    instance = this;

    onMouseDown = EventHandler<int>();
    onMouseUp = EventHandler<int>();

    glfwSetMouseButtonCallback(window, [](GLFWwindow * window, int button, int action, int mods) -> void {
        int id = 0;
        if (button == GLFW_MOUSE_BUTTON_LEFT) id = 0;
        if (button == GLFW_MOUSE_BUTTON_RIGHT) id = 1;
        if (button == GLFW_MOUSE_BUTTON_MIDDLE) id = 2;
        if (action == GLFW_PRESS) Mouse::instance->onMouseDown.invoke(id);
        if (action == GLFW_RELEASE) Mouse::instance->onMouseUp.invoke(id);
    });

}

void Mouse::setCursorMode(int mode)
{
    glfwSetInputMode(window, GLFW_CURSOR, mode);
}

std::tuple<float, float> Mouse::getCursorPosition()
{
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    return std::make_tuple(xpos, ypos);
}



Mouse::~Mouse()
{
    glfwSetMouseButtonCallback(window, NULL);
}
