#include "stdafx.h"
#include "Game.h"

Game * Game::instance = nullptr;

Game::Game(int windowwidth, int windowheight)
{
	instance = this;
	frame = 0;
	width = windowwidth;
	height = windowheight;
	invokeQueue = queue<function<void(void)>>();
	physicsInvokeQueue = queue<function<void(void)>>();
	textureMap = {};
	onRenderFrame = {};
	asset = new AssetLoader();
	world = new World();
	onRenderFrame = new EventHandler<int>();
	onRenderUIFrame = new EventHandler<int>();
	onWindowResize = new EventHandler<int>();
	onKeyPress = new EventHandler<int>();
	onKeyRelease = new EventHandler<int>();
	onKeyRepeat = new EventHandler<int>();
	onMouseDown = new EventHandler<int>();
	onMouseUp = new EventHandler<int>();
	onChar = new EventHandler<unsigned int>();
	idMap = unordered_map<int, void*>();
	hasExited = false;
}

Game::~Game()
{
}

void Game::start()
{
	vulkan = new VulkanToolkit();
	vulkan->initialize();
	renderer = new Renderer(width, height);
	thread renderthread(bind(&Game::renderThread, this));
	renderthread.detach();
}

unsigned int Game::getNextId()
{
	return lastId++;
}
void * Game::getObjectById(unsigned int id)
{
	return idMap[id];
}

void Game::registerId(unsigned int id, void * p)
{
	idMap[id] = p;
}

void Game::invoke(const function<void(void)> &func)
{
	invokeQueue.push(func);
}

void Game::physicsInvoke(const function<void(void)>& func)
{
	physicsInvokeQueue.push(func);
}

int Game::getKeyStatus(int key)
{
	return glfwGetKey(vulkan->window, key);
}

void Game::setCursorMode(int mode)
{
	glfwSetInputMode(vulkan->window, GLFW_CURSOR, mode);
}

glm::dvec2 Game::getCursorPosition()
{
	double xpos, ypos;
	glfwGetCursorPos(vulkan->window, &xpos, &ypos);
	return glm::dvec2(xpos, ypos);
}


void Game::glfwWindowSizeCallback(GLFWwindow* window, int w, int h)
{
	width = w;
	height = h;
//	renderer->resize(w, h);
}

void Game::physicsThread()
{
	double time = glfwGetTime();
	while (true)
	{
		while (Game::instance->physicsInvokeQueue.size() > 0) {
			Game::instance->physicsInvokeQueue.front()();
			Game::instance->physicsInvokeQueue.pop();
		}
		if (!Game::instance->physicsNeedsUpdate) {
			//    Sleep(4);
			continue;
		}

		double now = glfwGetTime();
		Game::instance->world->physics->simulationStep((float)((now - time)));
		time = now;
		Game::instance->physicsNeedsUpdate = false;
	}
}

void mousebuttoncallback(GLFWwindow * window, int button, int action, int mods) {

	int id = 0;
	if (button == GLFW_MOUSE_BUTTON_LEFT) id = 0;
	if (button == GLFW_MOUSE_BUTTON_RIGHT) id = 1;
	if (button == GLFW_MOUSE_BUTTON_MIDDLE) id = 2;
	if (action == GLFW_PRESS) Game::instance->onMouseDown->invoke(id);
	if (action == GLFW_RELEASE) Game::instance->onMouseUp->invoke(id);
	//  ImGui_ImplGlfwGL3_MouseButtonCallback(window, button, action, mods);
}

void Game::renderThread()
{

	glfwSetMouseButtonCallback(vulkan->window, mousebuttoncallback);

	glfwSetWindowSizeCallback(vulkan->window, [](GLFWwindow* window, int width, int height) -> void {
		instance->glfwWindowSizeCallback(window, width, height);
		instance->onWindowResize->invoke(0);
	});

	glfwSetCharCallback(vulkan->window, [](GLFWwindow* window, unsigned int key) -> void {
		instance->onChar->invoke(key);
	});



	shouldClose = false;
	int frames = 0;
	double lastTime = 0.0;
	//std::thread ptask(physicsThread);
	//ptask.detach();
	while (!glfwWindowShouldClose(vulkan->window) && !shouldClose)
	{
		frames++;
		double nowtime = floor(glfwGetTime());
		if (nowtime != lastTime) {
			printf("FPS %d\n", frames);
			frames = 0;
		}
		lastTime = nowtime;

		if (shouldClose) break;
		onRenderFrameFunc();

		glfwPollEvents();
	}
	delete vulkan;
	glfwTerminate();
	shouldClose = true;
	hasExited = true;
}


void Game::onRenderFrameFunc()
{
	firstFullDrawFinished = false;
	while (invokeQueue.size() > 0) {
		invokeQueue.front()();
		invokeQueue.pop();
	}
	onRenderFrame->invoke(0);

	if (world->mainDisplayCamera != nullptr) {
		if (vpmatrixUpdateFrameId != Game::instance->frame) {
			viewProjMatrix = world->mainDisplayCamera->projectionMatrix * world->mainDisplayCamera->transformation->getInverseWorldTransform();
			vpmatrixUpdateFrameId = Game::instance->frame;
		}
		renderer->renderToSwapChain(world->mainDisplayCamera);
	}
	onRenderUIFrame->invoke(0);
	physicsNeedsUpdate = true;
	frame++;
}