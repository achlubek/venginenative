#pragma once
#include <GLFW\glfw3.h>
#include "World.h";
#include "Renderer.h";
#include "AssetLoader.h";
#include "EventHandler.h";
 

class Game
{
public:

    World *world;
    Renderer *renderer;
    AssetLoader *asset;
    static Game *instance;
	VulkanToolkit* vulkan;
	VulkanImage* dummyTexture;

    int width;
    int height;
    int frame;

    Game(int windowwidth, int windowheight);
    ~Game();
    void start();
    void invoke(const function<void(void)> &func); 
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
    EventHandler<int> *onMouseDown;
    EventHandler<int> *onMouseUp;
    EventHandler<unsigned int> *onChar; 
    unsigned int getNextId();
    void* getObjectById(unsigned int id);
    void registerId(unsigned int id, void* p);
    glm::mat4 viewProjMatrix;
private:

    unsigned int lastId = 1;
    int vpmatrixUpdateFrameId = -1;

    queue<function<void(void)>> invokeQueue; 
    map<int, GLint> textureMap;
    void renderThread();
    void onRenderFrameFunc(); 
    unordered_map<int, void*> idMap;
};
