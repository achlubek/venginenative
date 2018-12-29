#include "stdafx.h"
#include "Keyboard.h"

namespace VEngine
{
    namespace Input
    {
        Keyboard * Keyboard::instance = nullptr;

        Keyboard::Keyboard(GLFWwindow* win)
            : window(win),
            onKeyPress(std::function<void(int)>()),
            onKeyRelease(std::function<void(int)>()),
            onKeyRepeat(std::function<void(int)>()),
            onChar(std::function<void(unsigned int)>())
        {
            instance = this;
            glfwSetCharCallback(window, [](GLFWwindow * window, unsigned int key) -> void {
                Keyboard::instance->onChar(key);
            });

            glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int, int action, int mods) -> void {
                if (action == GLFW_PRESS)
                    Keyboard::instance->onKeyPress(key);
                if (action == GLFW_RELEASE)
                    Keyboard::instance->onKeyRelease(key);
                if (action == GLFW_REPEAT)
                    Keyboard::instance->onKeyRepeat(key);
            });

        }

        void Keyboard::setOnKeyPressHandler(std::function<void(int)> ionKeyPress)
        {
            onKeyPress = ionKeyPress;
        }

        void Keyboard::setOnKeyReleaseHandler(std::function<void(int)> ionKeyRelease)
        {
            onKeyRelease = ionKeyRelease;
        }

        void Keyboard::setOnKeyRepeatHandler(std::function<void(int)> ionKeyRepeat)
        {
            onKeyRepeat = ionKeyRepeat;
        }

        void Keyboard::setOnCharHandler(std::function<void(unsigned int)> ionChar)
        {
            onChar = ionChar;
        }

        Keyboard::~Keyboard()
        {
            glfwSetCharCallback(window, NULL);
            glfwSetKeyCallback(window, NULL);
        }

        bool Keyboard::isKeyDown(int key)
        {
            return glfwGetKey(window, key) != GLFW_RELEASE;
        }
    }
}