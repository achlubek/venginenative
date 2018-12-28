#pragma once
#include <functional>

namespace VEngine
{
    namespace Input
    {
        class KeyboardInterface
        {
        public:
            virtual void setOnKeyPressHandler(std::function<void(int)> onKeyPress) = 0;
            virtual void setOnKeyReleaseHandler(std::function<void(int)> onKeyRelease) = 0;
            virtual void setOnKeyRepeatHandler(std::function<void(int)> onKeyRepeat) = 0;
            virtual void setOnCharHandler(std::function<void(unsigned int)> onChar) = 0;
            virtual bool isKeyDown(int key) = 0;
        };

    }
}