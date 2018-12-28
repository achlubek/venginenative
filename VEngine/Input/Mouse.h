#pragma once
namespace VEngine
{
    namespace Input
    {
        class Mouse
        {
        public:
            Mouse(GLFWwindow* win);
            ~Mouse();
            void setOnMouseDownHandler(std::function<void(int)> onMouseDown);
            void setOnMouseUpHandler(std::function<void(int)> onMouseUp);
            void setOnMouseScrollHandler(std::function<void(double, double)> onMouseScroll);
            void setCursorMode(int mode);
            std::tuple<double, double> getCursorPosition();
            bool isButtonPressed(int button);
        private:
            static Mouse * instance;
            GLFWwindow* window;
            int cursorMode = GLFW_CURSOR_NORMAL;
            std::function<void(int)> onMouseDown;
            std::function<void(int)> onMouseUp;
            std::function<void(double, double)> onMouseScroll;
        };

    }
}