#include "stdafx.h"
#include "VulkanRenderer.h"
#include "Application.h"


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

void VulkanRenderer::compile()
{
    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    if (vkCreateSemaphore(VulkanToolkit::singleton->device, &semaphoreInfo, nullptr, &imageAvailableSemaphore) != VK_SUCCESS) {
        throw std::runtime_error("failed to create semaphores!");
    }
    postprocessmesh = Application::instance->asset->loadObject3dInfoFile("ppplane.vbo");
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

void VulkanRenderer::draw()
{
    Application::instance->scene->prepareFrame();
    meshStage->beginDrawing();
    Application::instance->scene->draw(meshStage);
    meshStage->endDrawing();

    if (!ppRecorded) {
        for (int i = 0; i < postProcessingStages.size(); i++) {
            postProcessingStages[i]->beginDrawing();
            postProcessingStages[i]->drawMesh(postprocessmesh, *postProcessSet, 1);
            postProcessingStages[i]->endDrawing();
        }
        for (int i = 0; i < outputStages.size(); i++) {
            outputStages[i]->beginDrawing();
            outputStages[i]->drawMesh(postprocessmesh, *postProcessSet, 1);
            outputStages[i]->endDrawing();
        }

        ppRecorded = true;
    }

    uint32_t imageIndex;

    vkQueueWaitIdle(VulkanToolkit::singleton->mainQueue);

    vkAcquireNextImageKHR(VulkanToolkit::singleton->device, VulkanToolkit::singleton->swapChain->swapChain,
        std::numeric_limits<uint64_t>::max(), imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

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
