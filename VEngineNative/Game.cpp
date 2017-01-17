#include "stdafx.h"
#include "Game.h"
#include "imgui_impl_glfw_gl3.h"

Game * Game::instance = nullptr;

Game::Game(int windowwidth, int windowheight)
{
    instance = this;
    width = windowwidth;
    height = windowheight;
    invokeQueue = queue<function<void(void)>>();
    physicsInvokeQueue = queue<function<void(void)>>();
    textureMap = {};
    onRenderFrame = {};
    asset = new AssetLoader();
    world = new World();
    renderer = new Renderer(width, height);
    shaders = new GenericShaders();
    screenFbo = new Framebuffer(width, height, 0);
    onRenderFrame = new EventHandler<int>();
    onRenderUIFrame = new EventHandler<int>();
    onWindowResize = new EventHandler<int>();
    onKeyPress = new EventHandler<int>();
    onKeyRelease = new EventHandler<int>();
    onKeyRepeat = new EventHandler<int>();

    hasExited = false;
}

Game::~Game()
{
}

void Game::start()
{
    thread renderthread(bind(&Game::renderThread, this));
    renderthread.detach();
}

void Game::bindTexture(GLenum type, GLuint handle, int bindpoint)
{ 
    if (textureMap.find(bindpoint) == textureMap.end()) { 
        textureMap[bindpoint] = handle;
        glActiveTexture(GL_TEXTURE0 + bindpoint);
        glBindTexture(type, handle);
        glActiveTexture(GL_TEXTURE0 + 32);
    }
    else {
        if (textureMap[bindpoint] != handle) {
            textureMap[bindpoint] = handle;
            glActiveTexture(GL_TEXTURE0 + bindpoint);
            glBindTexture(type, handle);
            glActiveTexture(GL_TEXTURE0 + 32);
        }
        else {

        }
    }
}
unsigned int Game::getNextId()
{
    return lastId++;
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
    return glfwGetKey(window, key);
}

void Game::setCursorMode(int mode)
{
    glfwSetInputMode(window, GLFW_CURSOR, mode);
}

glm::dvec2 Game::getCursorPosition()
{
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    return glm::dvec2(xpos, ypos);
}

void APIENTRY debugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam)
{
    char debSource[160], debType[200], debSev[500];
    if (source == GL_DEBUG_SOURCE_API)
        strcpy_s(debSource, "OpenGL");
    else if (source == GL_DEBUG_SOURCE_WINDOW_SYSTEM)
        strcpy_s(debSource, "Window Sys");
    else if (source == GL_DEBUG_SOURCE_SHADER_COMPILER)
        strcpy_s(debSource, "Shader Compiler");
    else if (source == GL_DEBUG_SOURCE_THIRD_PARTY)
        strcpy_s(debSource, "Third Party");
    else if (source == GL_DEBUG_SOURCE_APPLICATION)
        strcpy_s(debSource, "Application");
    else if (source == GL_DEBUG_SOURCE_OTHER)
        strcpy_s(debSource, "Other");

    if (type == GL_DEBUG_TYPE_ERROR)
        strcpy_s(debType, "Error");
    else if (type == GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR)
        strcpy_s(debType, "Deprecated behavior");
    else if (type == GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR)
        strcpy_s(debType, "Undefined behavior");
    else if (type == GL_DEBUG_TYPE_PORTABILITY)
        strcpy_s(debType, "Portability");
    else if (type == GL_DEBUG_TYPE_PERFORMANCE)
        strcpy_s(debType, "Performance");
    else if (type == GL_DEBUG_TYPE_OTHER)
        strcpy_s(debType, "Other");

    if (severity == GL_DEBUG_SEVERITY_HIGH)
        strcpy_s(debSev, "High");
    else if (severity == GL_DEBUG_SEVERITY_MEDIUM)
        strcpy_s(debSev, "Medium");
    else if (severity == GL_DEBUG_SEVERITY_LOW)
        strcpy_s(debSev, "Low");

    printf("Source:%s\nType:%s\nID:%d\nSeverity:%s\nMessage:%s\n",
        debSource, debType, id, debSev, message);
}

void Game::glfwWindowSizeCallback(GLFWwindow* window, int w, int h)
{
    width = w;
    height = h;
    screenFbo = new Framebuffer(width, height, 0);
    renderer->resize(w, h);
}

void Game::display(string str)
{
    cout << str;
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
       // if (!Game::instance->physicsNeedsUpdate)
       //     continue;

        double now = glfwGetTime();
        Game::instance->world->physics->simulationStep((float)((now - time)));
        time = now;
        Game::instance->physicsNeedsUpdate = false;
    }
}

void Game::renderThread()
{
    if (!glfwInit()) {
        printf("ERROR: Cannot initialize GLFW!\n");
        return;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    //#ifdef _DEBUG
       // glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    //#endif
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //int count;
    //GLFWmonitor** monitors = glfwGetMonitors(&count);
    //GLFWmonitor* primary = glfwGetPrimaryMonitor();
   // const GLFWvidmode* mode = glfwGetVideoMode(primary);
   // glfwWindowHint(GLFW_RED_BITS, mode->redBits);
   // glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
   // glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
   // glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
    window = glfwCreateWindow(width, height, "VENGINE", NULL, NULL);
    // glfwSetWindowMonitor(window, primary, 0, 0, mode->width, mode->height, mode->refreshRate);
    glfwSetInputMode(window, GLFW_STICKY_KEYS, 1);
    if (!window)
    {
        printf("ERROR: Cannot create window or context!\n");
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGL())
    {
        printf("ERROR: Cannot initialize GLAD!\n");
        return;
    }

    glfwSetWindowSizeCallback(window, [](GLFWwindow* window, int width, int height) -> void {
        instance->glfwWindowSizeCallback(window, width, height);
        instance->onWindowResize->invoke(0);
    });

//    glfwSetKeyCallback(window, glfwKeyCallback);

    //#ifdef _DEBUG
      //  glDebugMessageCallback(&debugCallback, NULL);
    //#endif

    printf("VERSION: %s\nVENDOR: %s", glGetString(GL_VERSION), glGetString(GL_VENDOR));

    ImGui_ImplGlfwGL3_Init(window, true);

    glClearColor(0, 0, 0, 0);
    glClearDepth(1);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_DEPTH_CLAMP);
    glEnable(GL_CULL_FACE);
    glDepthFunc(GL_LEQUAL);
    glPatchParameteri(GL_PATCH_VERTICES, 3);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    shouldClose = false;
    double lastTime = glfwGetTime();
    int nbFrames = 0;
    std::thread ptask(physicsThread);
    ptask.detach();
    while (!glfwWindowShouldClose(window) && !shouldClose)
    {
        double currentTime = glfwGetTime();
        time = currentTime;
        nbFrames++;
        if (currentTime - lastTime >= 1.0) {
            printf("%f ms/frame = Frames %d\n", 1.0 / double(nbFrames), nbFrames);
            nbFrames = 0;
            lastTime = currentTime;
        }
        if (shouldClose) break;
        onRenderFrameFunc();

        glfwSwapBuffers(window);
    }
    ImGui_ImplGlfwGL3_Shutdown();
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
        vector<Light*> lights = world->scene->getLights();
        for (int i = 0; i < lights.size(); i++) {
            lights[i]->refreshShadowMap();
        }
        renderer->renderToFramebuffer(world->mainDisplayCamera, screenFbo);
    }
    ImGui_ImplGlfwGL3_NewFrame();
    glfwPollEvents();
    onRenderUIFrame->invoke(0);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    ImGui::Render();
    glDisable(GL_BLEND);
    physicsNeedsUpdate = true;
}