#include "stdafx.h"
#include "Keyboard.h"

namespace VEngine
{
    namespace Input
    {
        using namespace VEngine::Utilities;

        Keyboard * Keyboard::instance = nullptr;

        Keyboard::Keyboard(GLFWwindow* win)
        {
            window = win;
            instance = this;

            onKeyPress = EventHandler<int>();
            onKeyRelease = EventHandler<int>();
            onKeyRepeat = EventHandler<int>();
            onChar = EventHandler<unsigned int>();

            glfwSetCharCallback(window, [](GLFWwindow * window, unsigned int key) -> void {
                Keyboard::instance->onChar.invoke(key);
            });

            glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int, int action, int mods) -> void {
                if (action == GLFW_PRESS)
                    Keyboard::instance->onKeyPress.invoke(key);
                if (action == GLFW_RELEASE)
                    Keyboard::instance->onKeyRelease.invoke(key);
                if (action == GLFW_REPEAT)
                    Keyboard::instance->onKeyRepeat.invoke(key);
            });

        }

        Keyboard::~Keyboard()
        {
            glfwSetCharCallback(window, NULL);
            glfwSetKeyCallback(window, NULL);
        }

        int Keyboard::getKeyStatus(int key)
        {
            return glfwGetKey(window, key);
        }
    }
}