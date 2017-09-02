#pragma once
#include <GLFW\glfw3.h> 
#include "Camera.h";
#include "AssetLoader.h";
#include "EventHandler.h";
#include "Renderer.h";
 

class Application
{
public:
	static Application *instance;

    Scene *scene;
	AssetLoader *asset;
	Renderer *renderer;
	VulkanToolkit* vulkan;
	VulkanImage* dummyTexture;

	Camera * mainDisplayCamera = nullptr;

	int frame;
	int width;
	int height;

    Application(int windowwidth, int windowheight);
    ~Application();
    void start();
    void invoke(const function<void(void)> &func); 
    volatile bool shouldClose;
    volatile bool hasExited; 
    float time;

	EventHandler<int> onRenderFrame;
	EventHandler<int> onWindowResize;
    unsigned int getNextId();
    void* getObjectById(unsigned int id);
    void registerId(unsigned int id, void* p);
    glm::mat4 viewProjMatrix;
private:

    unsigned int lastId = 1;
    int vpmatrixUpdateFrameId = -1;

    queue<function<void(void)>> invokeQueue;   
    void onRenderFrameFunc();  
    unordered_map<int, void*> idMap;
};
