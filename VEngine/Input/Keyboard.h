#pragma once
namespace VEngine
{
    namespace Input
    {
        class Keyboard
        {
        public:
            Keyboard(GLFWwindow* win);
            void setOnKeyPressHandler(std::function<void(int)> onKeyPress);
            void setOnKeyReleaseHandler(std::function<void(int)> onKeyRelease);
            void setOnKeyRepeatHandler(std::function<void(int)> onKeyRepeat);
            void setOnCharHandler(std::function<void(unsigned int)> onChar);
            ~Keyboard();
            bool isKeyDown(int key);
        private:
            static Keyboard * instance;
            GLFWwindow* window;
            std::function<void(int)> onKeyPress;
            std::function<void(int)> onKeyRelease;
            std::function<void(int)> onKeyRepeat;
            std::function<void(unsigned int)> onChar;
        };

    }
}