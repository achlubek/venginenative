#include "stdafx.h"
#include "Application.h"
#include "Media.h"

Application * Application::instance = nullptr;

Application::Application(int windowwidth, int windowheight)
{
    mainDisplayCamera = new Camera();
    instance = this;
    frame = 0;
    width = windowwidth;
    height = windowheight;
    invokeQueue = queue<function<void(void)>>();
    onRenderFrame = {};
    scene = new Scene();
    onRenderFrame = EventHandler<int>();
    onWindowResize = EventHandler<int>();
    idMap = unordered_map<int, void*>();
    hasExited = false;
    vulkan = new VulkanToolkit();
    vulkan->initialize(windowwidth, windowheight, true, "VEngine old engine");
    //auto data = readFileImageData(path);
    asset = new AssetLoader(vulkan);
    ImageData img = ImageData();
    img.width = 1;
    img.height = 1;
    img.channelCount = 4;
    unsigned char * emptytexture = new unsigned char[4]{ (unsigned char)0x255, (unsigned char)0x255, (unsigned char)0x255, (unsigned char)0x255 };
    img.data = (void*)emptytexture;
    dummyTexture = vulkan->createTexture(img, VK_FORMAT_R8G8B8A8_UNORM);
    meshModelsDataLayout = new VulkanDescriptorSetLayout(vulkan);
    meshModelsDataLayout->addField(0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS);
    meshModelsDataLayout->compile();

    shadowMapDataLayout = new VulkanDescriptorSetLayout(vulkan);
    shadowMapDataLayout->addField(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS);
    shadowMapDataLayout->addField(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
    shadowMapDataLayout->compile();

    materialLayout = new VulkanDescriptorSetLayout(vulkan);

    materialLayout->addField(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS);

    materialLayout->addField(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
    materialLayout->addField(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
    materialLayout->addField(3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
    materialLayout->addField(4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
    materialLayout->addField(5, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
    materialLayout->compile();
    ui = new UIRenderer(vulkan, width, height);
    renderer = new Renderer(vulkan, width, height);
}

Application::~Application()
{
}

void Application::start()
{

    glfwSetWindowSizeCallback(vulkan->window, [](GLFWwindow* window, int width, int height) -> void {
        Application::instance->width = width;
        Application::instance->height = height;
    });

    shouldClose = false;
    int frames = 0;
    double lastTime = 0.0;
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

        vulkan->poolEvents();
    }
    delete vulkan;
    glfwTerminate();
    shouldClose = true;
    hasExited = true;
}

unsigned int Application::getNextId()
{
    return lastId++;
}
void * Application::getObjectById(unsigned int id)
{
    return idMap[id];
}

void Application::registerId(unsigned int id, void * p)
{
    idMap[id] = p;
}

void Application::invoke(const function<void(void)> &func)
{
    invokeQueue.push(func);
}

void Application::onRenderFrameFunc()
{
    if (mainDisplayCamera != nullptr) {
        time = glfwGetTime(); 

        if (vpmatrixUpdateFrameId != frame) {
            viewProjMatrix = mainDisplayCamera->projectionMatrix * mainDisplayCamera->transformation->getInverseWorldTransform();
            vpmatrixUpdateFrameId = frame;
        }

        while (invokeQueue.size() > 0) {
            invokeQueue.front()();
            invokeQueue.pop();
        }
        onRenderFrame.invoke(0);

        ui->draw();
        
        renderer->renderToSwapChain(mainDisplayCamera);

        frame++;
    }
}