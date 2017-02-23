#pragma once
#include <GLFW\glfw3.h>
#include "World.h";
#include "Renderer.h";
#include "GenericShaders.h";
#include "AssetLoader.h";
#include "EventHandler.h";

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

class Game
{
public:

    GLFWwindow *window;
    World *world;
    GenericShaders *shaders;
    Renderer *renderer;
    AssetLoader *asset;
    Framebuffer *screenFbo;
    static Game *instance;

    int width;
    int height;
    int frame;

    Game(int windowwidth, int windowheight);
    ~Game();
    void start();
    void invoke(const function<void(void)> &func);
    void physicsInvoke(const function<void(void)> &func);
    volatile bool shouldClose;
    volatile bool hasExited;
    volatile bool firstFullDrawFinished;
    float time;

    int getKeyStatus(int key);
    void setCursorMode(int mode);
    glm::dvec2 getCursorPosition();
    void glfwWindowSizeCallback(GLFWwindow* window, int width, int height);

    EventHandler<int> *onRenderFrame;
    EventHandler<int> *onRenderUIFrame;
    EventHandler<int> *onWindowResize;
    EventHandler<int> *onKeyPress;
    EventHandler<int> *onKeyRelease;
    EventHandler<int> *onKeyRepeat;
    EventHandler<unsigned int> *onChar;
    void bindTexture(GLenum type, GLuint handle, int bindpoint);
    volatile bool physicsNeedsUpdate = false;
    unsigned int getNextId();
    void* getObjectById(unsigned int id);
    void registerId(unsigned int id, void* p);
    glm::mat4 viewProjMatrix;
private:

    unsigned int lastId = 1;
    int vpmatrixUpdateFrameId = -1;

    queue<function<void(void)>> invokeQueue;
    queue<function<void(void)>> physicsInvokeQueue;
    map<int, GLint> textureMap;
    void renderThread();
    void onRenderFrameFunc();
    static void physicsThread();
    unordered_map<int, void*> idMap;
};
