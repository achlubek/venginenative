#include "stdafx.h"
#include "AbstractApp.h"
#include "Keyboard.h"
#include "Mouse.h"


AbstractApp::AbstractApp()
{
}


void AbstractApp::bind(Application * iapp)
{
    app = iapp;
    keyboard = new Keyboard(app->vulkan->window);
    mouse = new Mouse(app->vulkan->window);
    onBind();

    app->onRenderFrame.add([&](int zero) {
        onRenderFrame(app->time - lastTime);
        lastTime = app->time;
    });

    keyboard->onKeyPress.add([&](int key) {
        onKeyPress(key);
    });

    keyboard->onKeyRelease.add([&](int key) {
        onKeyRelease(key);
    });

    keyboard->onKeyRepeat.add([&](int key) {
        onKeyRepeat(key);
    });

    keyboard->onChar.add([&](unsigned int c) {
        onChar(c);
    });

    app->onWindowResize.add([&](int zero) {
        onWindowResize(app->width, app->height);
    });

    mouse->onMouseDown.add([&](int id) {
        onMouseDown(id);
    });

    mouse->onMouseUp.add([&](int id) {
        onMouseUp(id);
    });
}

AbstractApp::~AbstractApp()
{
}