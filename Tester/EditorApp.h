#pragma once
#include "AbstractApp.h"

#include "../VEngineNative/Camera.h";
#include "../VEngineNative/Object3dInfo.h";
#include "../VEngineNative/Media.h";
#include "../VEngineNative/World.h";
#include "../VEngineNative/Scene.h";
#include "../VEngineNative/Material.h";
#include "../VEngineNative/Mesh3d.h";
#include "../VEngineNative/Light.h";
#include "../VEngineNative/SquirrelVM.h";
#include "../VEngineNative/GLSLVM.h";
#include "../VEngineNative/SimpleParser.h";
#include "../VEngineNative/imgui/imgui.h";

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

    float yaw = 0.0f, pitch = 0.0f;
    double lastcx = 0.0f, lastcy = 0.0f;
    Camera* cam;
    Mesh3d * pickedUpMesh = nullptr;
    Mesh3dLodLevel * pickedUpMeshLodLevel = nullptr;
    Mesh3dInstance * pickedUpMeshInstance = nullptr;
    vector<string> commandHistory;
    string currentCommandText;
    bool isConsoleWindowOpened = false, isPickingWindowOpened = false,
        transformationWindowOpened = false;

    int currentMode = EDITOR_MODE_IDLE;
    int lastMode = EDITOR_MODE_IDLE;

    void switchMode(int mode);

    virtual void initialize() override;

    virtual void onRenderFrame(float elapsed) override;

    virtual void onRenderUIFrame(float elapsed) override;

    virtual void onWindowResize(int width, int height) override;

    virtual void onKeyPress(int key) override;

    virtual void onKeyRelease(int key) override;

    virtual void onKeyRepeat(int key) override;

    virtual void onBind() override;

    virtual void onChar(unsigned int c) override;

    void onCommandText(string text);

private:
    bool ignoreNextChar = false;

};

