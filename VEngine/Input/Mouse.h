#pragma once
#include "../Interface/Input/MouseInterface.h"

namespace VEngine
{
    namespace Input
    {
        class Mouse : public MouseInterface
        {
        public:
            Mouse(GLFWwindow* win);
            ~Mouse();
            virtual void setOnMouseDownHandler(std::function<void(int)> onMouseDown) override;
            virtual void setOnMouseUpHandler(std::function<void(int)> onMouseUp) override;
            virtual void setOnMouseScrollHandler(std::function<void(double, double)> onMouseScroll) override;
            virtual void setCursorMode(int mode) override;
            virtual std::tuple<double, double> getCursorPosition() override;
            virtual bool isButtonPressed(int button) override;
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