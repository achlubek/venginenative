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
    game->onWindowResize->add([&](int zero) {
        onWindowResize(game->width, game->height);
    });
}

AbstractApp::~AbstractApp()
{
}
