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
Object3dInfo * createPostProcessingObject()
{ 
    unsigned char bytes[288] = {
        0x00, 0x00, 0x80, 0x3F, 0x00, 0x00, 0x80, 0xBF, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x80, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x3F, 0x00, 0x00, 0x80, 0xBF,
        0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x80, 0x3F,
        0x00, 0x00, 0x80, 0xBF, 0x00, 0x00, 0x80, 0x3F, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x3F, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x3F, 0x00, 0x00, 0x80, 0xBF,
        0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x80, 0x3F,
        0x00, 0x00, 0x80, 0xBF, 0x00, 0x00, 0x80, 0xBF, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x3F, 0x00, 0x00, 0x80, 0xBF,
        0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x80, 0x3F,
        0x00, 0x00, 0x80, 0x3F, 0x00, 0x00, 0x80, 0xBF, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x80, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x3F, 0x00, 0x00, 0x80, 0xBF,
        0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x80, 0x3F,
        0x00, 0x00, 0x80, 0x3F, 0x00, 0x00, 0x80, 0x3F, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x80, 0x3F, 0x00, 0x00, 0x80, 0x3F, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x3F, 0x00, 0x00, 0x80, 0xBF,
        0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x80, 0x3F,
        0x00, 0x00, 0x80, 0xBF, 0x00, 0x00, 0x80, 0x3F, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x3F, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x3F, 0x00, 0x00, 0x80, 0xBF,
        0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x80, 0x3F
    };

    int bytescount = 288;
    GLfloat * floats = (GLfloat*)bytes;
    int floatsCount = bytescount / 4;
    vector<GLfloat> flo(floats, floats + floatsCount);

    auto o = new Object3dInfo(flo);
    return o;

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
    postprocessmesh = createPostProcessingObject();
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
            outputStages[i]->addOutputImage(*img);

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
    VkPipelineStageFlags waitStages2[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = waitSemaphores.size();
    submitInfo.pWaitSemaphores = waitSemaphores.data();
    submitInfo.pWaitDstStageMask = waitStages2;

    submitInfo.commandBufferCount = 0;
    submitInfo.pCommandBuffers = nullptr;

    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &signalSemaphore;

    if (vkQueueSubmit(VulkanToolkit::singleton->mainQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit draw command buffer!");
    }
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
    }
    vkQueueWaitIdle(VulkanToolkit::singleton->mainQueue);
}
