#pragma once
class Game;

class AbstractApp
{
public:
    AbstractApp();

    virtual void initialize() = 0;
    virtual void onRenderFrame(float elapsed) = 0;
    virtual void onRenderUIFrame(float elapsed) = 0;
    virtual void onWindowResize(int width, int height) = 0;
    virtual void onKeyPress(int key) = 0;
    virtual void onKeyRelease(int key) = 0;
    virtual void onKeyRepeat(int key) = 0;
    virtual void onBind() = 0;
    void bind(Game* game);
    ~AbstractApp();

    int width = 0, height = 0;
    Game* game;
private:
    float lastTime, lastTimeUI;
};

