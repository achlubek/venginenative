#pragma once
#include "AbstractApp.h"
#include "../VEngineNative/stdafx.h"
#include "../VEngineNative/Game.h"
#include "../VEngineNative/Media.h"
#include "../VEngineNative/Camera.h";
#include "../VEngineNative/Object3dInfo.h";
#include "../VEngineNative/World.h";
#include "../VEngineNative/Scene.h";
#include "../VEngineNative/Material.h";
#include "../VEngineNative/Sound.h";
#include "../VEngineNative/Sound3d.h";
#include "../VEngineNative/Mesh3d.h";
#include "../VEngineNative/SquirrelVM.h";
#include "../VEngineNative/SimpleParser.h";

#include "Car.h";

#define EDITOR_MODE_IDLE 0
#define EDITOR_MODE_MOVE_CAMERA 1
#define EDITOR_MODE_PICKING 2
#define EDITOR_MODE_EDITING 3
#define EDITOR_MODE_WRITING_TEXT 4

class EditorApp : public AbstractApp
{
public:
	EditorApp();
	~EditorApp();
	 
	virtual void initialize() override;
	virtual void onRenderFrame(float elapsed) override;
	virtual void onRenderUIFrame(float elapsed) override;
	virtual void onWindowResize(int width, int height) override;
	virtual void onKeyPress(int key) override;
	virtual void onKeyRelease(int key) override;
	virtual void onKeyRepeat(int key) override;
	virtual void onMouseDown(int button) override;
	virtual void onMouseUp(int button) override;
	virtual void onChar(unsigned int c) override;
	virtual void onBind() override;

	void switchMode(int mode);


	int currentMode = EDITOR_MODE_IDLE;
	int lastMode = EDITOR_MODE_IDLE;

	float yaw = 0.0f, pitch = 0.0f;
	double lastcx = 0.0f, lastcy = 0.0f;
	Camera* cam;
	Sound3d* testsound, *testsound2, *wind, *ocean1, *ocean2, *ocean3, *ocean4;

    float FOV = 10.0f;
	glm::quat backquat = glm::quat();
	glm::vec3 backvectorlast = glm::vec3();
	glm::vec3 lastpos = glm::vec3();
	bool cameraFollowCar = false;
	vector<Car *> car;
};

