#pragma once
#include "../Utilities/EventHandler.h"
namespace VEngine
{
    namespace Input
    {
        class Keyboard
        {
        public:
            Keyboard(GLFWwindow* win);
            ~Keyboard();
            VEngine::Utilities::EventHandler<int> onKeyPress;
            VEngine::Utilities::EventHandler<int> onKeyRelease;
            VEngine::Utilities::EventHandler<int> onKeyRepeat;
            VEngine::Utilities::EventHandler<unsigned int> onChar;
            int getKeyStatus(int key);
        private:
            static Keyboard * instance;
            GLFWwindow* window;
        };

    }
}