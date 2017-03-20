#pragma once
#include "AbstractApp.h"

#include "../VEngineNative/Camera.h";
#include "../VEngineNative/Object3dInfo.h";
#include "../VEngineNative/Media.h";
#include "../VEngineNative/World.h";
#include "../VEngineNative/Scene.h";
#include "../VEngineNative/Material.h";
#include "../VEngineNative/Sound.h";
#include "../VEngineNative/Sound3d.h";
#include "../VEngineNative/Mesh3d.h";
#include "../VEngineNative/Light.h";
#include "../VEngineNative/SquirrelVM.h";
#include "../VEngineNative/GLSLVM.h";
#include "../VEngineNative/SimpleParser.h";
#include "../VEngineNative/imgui/imgui.h";

#include "Car.h"

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
    Mesh3d * pickedUpMesh = nullptr, *mrstick;
    Mesh3dLodLevel * pickedUpMeshLodLevel = nullptr;
    Mesh3dInstance * pickedUpMeshInstance = nullptr;
    vector<string> commandHistory;
    string currentCommandText;
    Mesh3d * cursor3dArrow;
    bool isConsoleWindowOpened = false, isPickingWindowOpened = false,
        transformationWindowOpened = false;
    bool customWindowsOpened[10] = { false, false, false, false, false, false, false, false, false, false};
    bool isOpened = true;
    bool isOpened2 = true;
    float tmpDayElapsed = 0.5;
    vector<Car *> car;
    vector<glm::vec3> bonesBinds;
    vector<PhysicalBody *> bonesBodies;
    vector<PhysicalConstraint *> bonesConstrs;

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

    Sound3d* testsound, *testsound2, *wind, *ocean1, *ocean2, *ocean3, *ocean4;

    float* bytes2;

    glm::quat backquat = glm::quat();
    glm::vec3 backvectorlast = glm::vec3();
    glm::vec3 lastpos = glm::vec3();
    bool cameraFollowCar = false;

   // PhysicalBody* virtualbox;

};

