#pragma once
#include "EventHandler.h"
class Mouse
{
public:
    Mouse(GLFWwindow* win);
    ~Mouse();
    EventHandler<int> onMouseDown;
    EventHandler<int> onMouseUp;
    EventHandler<int> onMouseScroll;
    void setCursorMode(int mode);
    std::tuple<float, float> getCursorPosition();
    bool isButtonPressed(int button);
private:
    static Mouse * instance;
    GLFWwindow* window;
    int cursorMode = GLFW_CURSOR_NORMAL;
};

