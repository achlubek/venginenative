#pragma once
#include "EventHandler.h"
class Keyboard
{
public:
    Keyboard(GLFWwindow* win);
    ~Keyboard();
    EventHandler<int> onKeyPress;
    EventHandler<int> onKeyRelease;
    EventHandler<int> onKeyRepeat; 
    EventHandler<unsigned int> onChar;  
    int getKeyStatus(int key);
private:
    static Keyboard * instance;
    GLFWwindow* window;
};

