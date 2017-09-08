#include "stdafx.h" 


VulkanRenderer::VulkanRenderer()
{
    postProcessingStages = {};
    outputStages = {};
}


VulkanRenderer::~VulkanRenderer()
{
    vkDestroySemaphore(VulkanToolkit::singleton->device, imageAvailableSemaphore, nullptr);
}

void VulkanRenderer::setMeshStage(VulkanRenderStage * stage)
{
    usingMeshStage = true;
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

void VulkanRenderer::compile()
{
    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    if (vkCreateSemaphore(VulkanToolkit::singleton->device, &semaphoreInfo, nullptr, &imageAvailableSemaphore) != VK_SUCCESS) {
        throw std::runtime_error("failed to create semaphores!");
    }
    if (vkCreateSemaphore(VulkanToolkit::singleton->device, &semaphoreInfo, nullptr, &stubMeshSemaphore) != VK_SUCCESS) {
        throw std::runtime_error("failed to create semaphores!");
    }
    postprocessmesh = VulkanToolkit::singleton->fullScreenQuad3dInfo;
    if (usingMeshStage) {
        meshStage->compile();
    }

    for (int i = 0; i < postProcessingStages.size(); i++) {
        postProcessingStages[i]->compile();
    }
    if (outputStageZygote != nullptr) {
        VkFormat format = VulkanToolkit::singleton->swapChain->swapChainImageFormat;
        outputStages.resize(VulkanToolkit::singleton->swapChain->swapChainImages.size());
        for (int i = 0; i < VulkanToolkit::singleton->swapChain->swapChainImages.size(); i++) {

            outputStages[i] = outputStageZygote->copy();

            VulkanImage* img = new VulkanImage(format, VulkanToolkit::singleton->swapChain->swapChainImages[i], VulkanToolkit::singleton->swapChain->swapChainImageViews[i]);
            img->format = format;
            img->image = VulkanToolkit::singleton->swapChain->swapChainImages[i];
            img->imageView = VulkanToolkit::singleton->swapChain->swapChainImageViews[i];
            img->isPresentReady = true;
            outputStages[i]->addOutputImage(img);

            outputStages[i]->compile();
        }
    }
}

void VulkanRenderer::beginDrawing()
{
    if (usingMeshStage) {
        meshStage->beginDrawing();
    }
}


void VulkanRenderer::submitEmptyBatch(std::vector<VkSemaphore> waitSemaphores, VkSemaphore signalSemaphore)
{
    VkPipelineStageFlags waitStages2[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };// THIS GOT TO HAVE count of ALL wait stages NOT ONLY ONE ELEMENT
    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = waitSemaphores.size();
    submitInfo.pWaitSemaphores = waitSemaphores.data();
    submitInfo.pWaitDstStageMask = waitStages2;

    submitInfo.commandBufferCount = 0;
    submitInfo.pCommandBuffers = nullptr;

    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &signalSemaphore;

    vkQueueSubmit(VulkanToolkit::singleton->mainQueue, 1, &submitInfo, VK_NULL_HANDLE);
}

void VulkanRenderer::submitEmptyBatch2(std::vector<VkSemaphore> waitSemaphores)
{
    VkPipelineStageFlags waitStages2[] = { VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT };// THIS GOT TO HAVE count of ALL wait stages NOT ONLY ONE ELEMENT
    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = waitSemaphores.size();
    submitInfo.pWaitSemaphores = waitSemaphores.data();
    submitInfo.pWaitDstStageMask = waitStages2;

    submitInfo.commandBufferCount = 0;
    submitInfo.pCommandBuffers = nullptr;

    submitInfo.signalSemaphoreCount = 0;
    submitInfo.pSignalSemaphores = nullptr;

    vkQueueSubmit(VulkanToolkit::singleton->mainQueue, 1, &submitInfo, VK_NULL_HANDLE);
}

void VulkanRenderer::submitEmptyBatch3(VkSemaphore signalSemaphore)
{
    VkPipelineStageFlags waitStages2[] = { VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT }; // THIS GOT TO HAVE count of ALL wait stages NOT ONLY ONE ELEMENT
    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = 0;
    submitInfo.pWaitSemaphores = nullptr;
    submitInfo.pWaitDstStageMask = waitStages2;

    submitInfo.commandBufferCount = 0;
    submitInfo.pCommandBuffers = nullptr;

    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &signalSemaphore;

    vkQueueSubmit(VulkanToolkit::singleton->mainQueue, 1, &submitInfo, VK_NULL_HANDLE);
}

void VulkanRenderer::endDrawing()
{
    if (usingMeshStage) {
        meshStage->endDrawing();
    }

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
    vkQueueWaitIdle(VulkanToolkit::singleton->mainQueue);
    if (outputStageZygote != nullptr) {
        uint32_t imageIndex;

        vkAcquireNextImageKHR(VulkanToolkit::singleton->device, VulkanToolkit::singleton->swapChain->swapChain,
            9999999, imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

        VkSemaphore lastSemaphore = stubMeshSemaphore;
        if (usingMeshStage) {
            meshStage->submit({ imageAvailableSemaphore });
            lastSemaphore = meshStage->signalSemaphore;
        }
        else {
            submitEmptyBatch({ imageAvailableSemaphore }, stubMeshSemaphore);
            lastSemaphore = stubMeshSemaphore;
        }

        for (int i = 0; i < postProcessingStages.size(); i++) {
            postProcessingStages[i]->submit({ lastSemaphore });
            lastSemaphore = postProcessingStages[i]->signalSemaphore;
        }

        outputStages[imageIndex]->submit({ lastSemaphore });

        VulkanToolkit::singleton->swapChain->present({ outputStages[imageIndex]->signalSemaphore }, imageIndex);

    }
    else {
        VkSemaphore lastSemaphore = stubMeshSemaphore;
        if (usingMeshStage) { 
            meshStage->submit({ });
            lastSemaphore = meshStage->signalSemaphore;
        }
        else {
            submitEmptyBatch({ }, stubMeshSemaphore);
            lastSemaphore = stubMeshSemaphore;
        }

        for (int i = 0; i < postProcessingStages.size(); i++) {
            postProcessingStages[i]->submit({ lastSemaphore });
            lastSemaphore = postProcessingStages[i]->signalSemaphore;
        }

        submitEmptyBatch2({ meshStage->signalSemaphore });
    }
    vkQueueWaitIdle(VulkanToolkit::singleton->mainQueue);
}
