#pragma once
#include <tuple>
#include <functional>

namespace VEngine
{
    namespace Input
    {
        class MouseInterface
        {
        public:
            virtual void setOnMouseDownHandler(std::function<void(int)> onMouseDown) = 0;
            virtual void setOnMouseUpHandler(std::function<void(int)> onMouseUp) = 0;
            virtual void setOnMouseScrollHandler(std::function<void(double, double)> onMouseScroll) = 0;
            virtual void setCursorMode(int mode) = 0;
            virtual std::tuple<double, double> getCursorPosition() = 0;
            virtual bool isButtonPressed(int button) = 0;
        };

    }
}