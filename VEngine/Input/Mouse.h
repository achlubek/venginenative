#pragma once
#include "../Utilities/EventHandler.h"

namespace VEngine
{
    namespace Input
    {
        class Mouse
        {
        public:
            Mouse(GLFWwindow* win);
            ~Mouse();
            VEngine::Utilities::EventHandler<int> onMouseDown;
            VEngine::Utilities::EventHandler<int> onMouseUp;
            VEngine::Utilities::EventHandler<double> onMouseScroll;
            void setCursorMode(int mode);
            std::tuple<double, double> getCursorPosition();
            bool isButtonPressed(int button);
        private:
            static Mouse * instance;
            GLFWwindow* window;
            int cursorMode = GLFW_CURSOR_NORMAL;
        };

    }
}