#pragma once
#include "EventHandler.h"
class Mouse
{
public:
	Mouse(GLFWwindow* win);
	~Mouse();
	EventHandler<int> onMouseDown;
	EventHandler<int> onMouseUp;
	void setCursorMode(int mode);
	std::tuple<float, float> getCursorPosition();
private:
	static Mouse * instance;
	GLFWwindow* window;
};

