#pragma once
#include "../Interface/Input/KeyboardInterface.h"

namespace VEngine
{
    namespace Input
    {
        class Keyboard : public KeyboardInterface
        {
        public:
            Keyboard(GLFWwindow* win);
            ~Keyboard();
            virtual void setOnKeyPressHandler(std::function<void(int)> onKeyPress) override;
            virtual void setOnKeyReleaseHandler(std::function<void(int)> onKeyRelease) override;
            virtual void setOnKeyRepeatHandler(std::function<void(int)> onKeyRepeat) override;
            virtual void setOnCharHandler(std::function<void(unsigned int)> onChar) override;
            virtual bool isKeyDown(int key) override;
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