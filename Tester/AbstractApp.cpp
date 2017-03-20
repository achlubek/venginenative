#include "stdafx.h"
#include "AbstractApp.h"


AbstractApp::AbstractApp()
{
}


void AbstractApp::bind(Game * igame)
{
    game = igame;
    onBind();
    game->onRenderFrame->add([&](int zero) {
        onRenderFrame(game->time - lastTime);
        lastTime = game->time;
    });
    game->onRenderUIFrame->add([&](int zero) {
        onRenderUIFrame(game->time - lastTimeUI);
        lastTimeUI = game->time;
    });
    game->onKeyPress->add([&](int key) {
        onKeyPress(key);
    });
    game->onKeyRelease->add([&](int key) {
        onKeyRelease(key);
    });
    game->onKeyRepeat->add([&](int key) {
        onKeyRepeat(key);
    });
    game->onChar->add([&](unsigned int c) {
        onChar(c);
    });
    game->onWindowResize->add([&](int zero) {
        onWindowResize(game->width, game->height);
    });
    game->onMouseDown->add([&](int id) {
        onMouseDown(id);
    });
    game->onMouseUp->add([&](int id) {
        onMouseUp(id);
    });
}

AbstractApp::~AbstractApp()
{
}
