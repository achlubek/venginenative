#pragma once
#include "../Interface/Input/JoystickInterface.h"

namespace VEngine
{
    namespace Input
    {
        class Joystick : public JoystickInterface
        {
        public:
            Joystick(GLFWwindow* win);
            ~Joystick();
            virtual std::vector<bool> getButtonsStatus(int index) override;
            virtual bool isPresent(int index) override;
            virtual std::vector<float> getAxes(int index) override;
        private:
            GLFWwindow* window;
        };

    }
}