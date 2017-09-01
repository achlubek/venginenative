#pragma once
class Application;
class Keyboard;
class Mouse;

class AbstractApp
{
public:
	AbstractApp();

	virtual void initialize() = 0;
	virtual void onRenderFrame(float elapsed) = 0; 
	virtual void onWindowResize(int width, int height) = 0;
	virtual void onKeyPress(int key) = 0;
	virtual void onKeyRelease(int key) = 0;
	virtual void onKeyRepeat(int key) = 0;
	virtual void onMouseDown(int button) = 0;
	virtual void onMouseUp(int button) = 0;
	virtual void onChar(unsigned int c) = 0;
	virtual void onBind() = 0;
	void bind(Application* iapp);
	~AbstractApp();

	Keyboard * keyboard;
	Mouse * mouse;

	int width = 0, height = 0;
	Application* app;
private:
	float lastTime, lastTimeUI;
};
