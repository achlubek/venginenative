#pragma once
#include "AbstractApp.h"
class EditorApp : public AbstractApp
{
public:
    EditorApp();
    ~EditorApp();

    bool cursorFree = false;
    float yaw = 0.0f, pitch = 0.0f;
    double lastcx = 0.0f, lastcy = 0.0f;
    Camera* cam;

    virtual void initialize() override;

    virtual void onRenderFrame(float elapsed) override;

    virtual void onRenderUIFrame(float elapsed) override;

    virtual void onWindowResize(int width, int height) override;

    virtual void onKeyPress(int key) override;

    virtual void onKeyRelease(int key) override;

    virtual void onKeyRepeat(int key) override;

    virtual void onBind() override;

};

