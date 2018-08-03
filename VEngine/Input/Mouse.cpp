#include "stdafx.h"
#include "Mouse.h"

namespace VEngine
{
    namespace Input
    {
        using namespace VEngine::Utilities;

        Mouse * Mouse::instance = nullptr;

        Mouse::Mouse(GLFWwindow* win)
        {
            window = win;
            instance = this;

            onMouseDown = EventHandler<int>();
            onMouseUp = EventHandler<int>();
            onMouseScroll = EventHandler<double>();

            glfwSetMouseButtonCallback(window, [](GLFWwindow * window, int button, int action, int mods) -> void {
                int id = 0;
                if (button == GLFW_MOUSE_BUTTON_LEFT) id = 0;
                if (button == GLFW_MOUSE_BUTTON_RIGHT) id = 1;
                if (button == GLFW_MOUSE_BUTTON_MIDDLE) id = 2;
                if (action == GLFW_PRESS) Mouse::instance->onMouseDown.invoke(id);
                if (action == GLFW_RELEASE) Mouse::instance->onMouseUp.invoke(id);
            });
            glfwSetScrollCallback(window, [](GLFWwindow * window, double xoffset, double yoffset) -> void {
                Mouse::instance->onMouseScroll.invoke(yoffset); // TODO add x offset making another callback
            });
            glfwSetInputMode(window, GLFW_CURSOR, cursorMode);
        }

        void Mouse::setCursorMode(int mode)
        {
            if (mode != cursorMode) {
                glfwSetInputMode(window, GLFW_CURSOR, mode);
                cursorMode = mode;
            }
        }

        std::tuple<double, double> Mouse::getCursorPosition()
        {
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            return std::make_tuple(xpos, ypos);
        }

        bool Mouse::isButtonPressed(int button)
        {
            return glfwGetMouseButton(window, button) == GLFW_PRESS;
        }

        Mouse::~Mouse()
        {
            glfwSetMouseButtonCallback(window, NULL);
        }

    }
}