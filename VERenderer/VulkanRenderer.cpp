#include "stdafx.h" 


VulkanRenderer::VulkanRenderer()
{
    meshStage = nullptr;
    postProcessingStages = {};
    outputStages = {};
}


VulkanRenderer::~VulkanRenderer()
{
    vkDestroySemaphore(VulkanToolkit::singleton->device, imageAvailableSemaphore, nullptr);
}

void VulkanRenderer::setMeshStage(VulkanRenderStage * stage)
{
    meshStage = stage;
}

void VulkanRenderer::setOutputStage(VulkanRenderStage * stage)
{
    outputStageZygote = stage;
}

void VulkanRenderer::addPostProcessingStage(VulkanRenderStage * stage)
{
    postProcessingStages.push_back(stage);
}

void VulkanRenderer::setPostProcessingDescriptorSet(VulkanDescriptorSet * set)
{
    postProcessSet = set;
}
VulkanRenderStage * VulkanRenderer::getMesh3dStage()
{
    return meshStage;
}
Object3dInfo * loadObject3dInfoFile(string source)
{
    void * cached = Media::checkCache(source);
    if (cached != nullptr) {
        return (Object3dInfo*)cached;
    }
    unsigned char* bytes;
    int bytescount = Media::readBinary(source, &bytes);
    GLfloat * floats = (GLfloat*)bytes;
    int floatsCount = bytescount / 4;
    vector<GLfloat> flo(floats, floats + floatsCount);

    auto o = new Object3dInfo(flo);
    Media::saveCache(source, o);
    return o;

}
void VulkanRenderer::compile()
{
    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    if (vkCreateSemaphore(VulkanToolkit::singleton->device, &semaphoreInfo, nullptr, &imageAvailableSemaphore) != VK_SUCCESS) {
        throw std::runtime_error("failed to create semaphores!");
    }
    postprocessmesh = loadObject3dInfoFile("ppplane.vbo");
    meshStage->compile();

    for (int i = 0; i < postProcessingStages.size(); i++) {
        postProcessingStages[i]->compile();
    }

    VkFormat format = VulkanToolkit::singleton->swapChain->swapChainImageFormat;
    outputStages.resize(VulkanToolkit::singleton->swapChain->swapChainImages.size());
    for (int i = 0; i < VulkanToolkit::singleton->swapChain->swapChainImages.size(); i++) {

        outputStages[i] = outputStageZygote->copy();

        VulkanImage* img = new VulkanImage(format, VulkanToolkit::singleton->swapChain->swapChainImages[i], VulkanToolkit::singleton->swapChain->swapChainImageViews[i]);
        img->format = format;
        img->image = VulkanToolkit::singleton->swapChain->swapChainImages[i];
        img->imageView = VulkanToolkit::singleton->swapChain->swapChainImageViews[i];
        img->isPresentReady = true;
        outputStages[i]->addOutputImage(*img);

        outputStages[i]->compile();
    }
}

void VulkanRenderer::beginDrawing()
{
    meshStage->beginDrawing();
}
void VulkanRenderer::endDrawing()
{
    meshStage->endDrawing();

    if (!ppRecorded) {
        for (int i = 0; i < postProcessingStages.size(); i++) {
            postProcessingStages[i]->beginDrawing();
            postProcessingStages[i]->drawMesh(postprocessmesh, { *postProcessSet }, 1);
            postProcessingStages[i]->endDrawing();
        }
        for (int i = 0; i < outputStages.size(); i++) {
            outputStages[i]->beginDrawing();
            outputStages[i]->drawMesh(postprocessmesh, { *postProcessSet }, 1);
            outputStages[i]->endDrawing();
        }

        ppRecorded = true;
    }

    uint32_t imageIndex;

    vkQueueWaitIdle(VulkanToolkit::singleton->mainQueue);

    vkAcquireNextImageKHR(VulkanToolkit::singleton->device, VulkanToolkit::singleton->swapChain->swapChain,
        9999999, imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

    meshStage->submit({ imageAvailableSemaphore });
    VkSemaphore lastSemaphore = meshStage->signalSemaphore;

    for (int i = 0; i < postProcessingStages.size(); i++) {
        postProcessingStages[i]->submit({ lastSemaphore });
        lastSemaphore = postProcessingStages[i]->signalSemaphore;
    }

    outputStages[imageIndex]->submit({ lastSemaphore });

    VulkanToolkit::singleton->swapChain->present({ outputStages[imageIndex]->signalSemaphore }, imageIndex);

    vkQueueWaitIdle(VulkanToolkit::singleton->mainQueue);
}
