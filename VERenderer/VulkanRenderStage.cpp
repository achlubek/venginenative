#include "stdafx.h"
#include "Object3dInfo.h"

VulkanRenderStage::VulkanRenderStage()
{
    setLayouts = {};
    outputImages = {};
    shaderStages = {};
}


VulkanRenderStage::~VulkanRenderStage()
{
}

void VulkanRenderStage::addDescriptorSetLayout(VkDescriptorSetLayout lay)
{
    setLayouts.push_back(lay);
}

void VulkanRenderStage::addOutputImage(VulkanImage * lay)
{
    outputImages.push_back(lay);
}

void VulkanRenderStage::setViewport(VkExtent2D size)
{
    viewport = size;
}

void VulkanRenderStage::setViewport(int width, int height)
{
    VkExtent2D e = VkExtent2D();
    e.width = width;
    e.height = height;
    setViewport(e);
}

void VulkanRenderStage::addShaderStage(VkPipelineShaderStageCreateInfo ss)
{
    shaderStages.push_back(ss);
}

// limit eg 2 so values 0 1 2
int getLastIndex(int current, int limit) {
    return current == 0 ? limit : current - 1;
}

void VulkanRenderStage::beginDrawing()
{
    cmdMeshesCounts = 0;
    commandBuffer.begin(VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT);

    VkRenderPassBeginInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass.handle;
    renderPassInfo.framebuffer = framebuffer.handle;
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = viewport;

    std::vector<VkClearValue> clearValues = {};

    for (int i = 0; i < renderPass.attachments.size(); i++) {
        VkClearValue c = VkClearValue();
        if (renderPass.attachments[i].image->isDepthBuffer) {
            c.depthStencil = { 1.0f, 0 };
        }
        else {
            c.color = { 0.0f, 0.0f, 0.0f, 0.0f };
        }
        clearValues.push_back(c);
    }
    
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffer.handle, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(commandBuffer.handle, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.graphicsPipeline);

}
void VulkanRenderStage::endDrawing()
{


    vkCmdEndRenderPass(commandBuffer.handle);

    commandBuffer.end();
}

void VulkanRenderStage::drawMesh(Object3dInfo * info, std::vector<VulkanDescriptorSet> sets, size_t instances)
{
    info->drawInstanced(pipeline, sets, commandBuffer.handle, instances);
    cmdMeshesCounts++;
}

void VulkanRenderStage::compile()
{ 
    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    vkCreateSemaphore(VulkanToolkit::singleton->device, &semaphoreInfo, nullptr, &signalSemaphore);

    commandBuffer = VulkanCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY);

    std::vector<VulkanAttachment> attachments = {};
    std::vector<VkImageView> attachmentsViews = {};
    std::vector<VulkanAttachment> colorAttachments = { };
    VulkanAttachment depthAttachment = VulkanAttachment();
    std::vector<VkImageLayout> colorattachmentsLayouts = { };

    bool foundDepthBuffer = false;
    for (int i = 0; i < outputImages.size(); i++) {
        auto atta = outputImages[i]->getAttachment();
        if (outputImages[i]->isDepthBuffer) {
            depthAttachment = atta;
            foundDepthBuffer = true;
        }
        else {
            colorAttachments.push_back(atta);
            colorattachmentsLayouts.push_back(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
        }
        attachments.push_back(atta);
        attachmentsViews.push_back(outputImages[i]->imageView);
    }

    VulkanSubpass subpass;
    if (foundDepthBuffer) subpass = VulkanSubpass(colorAttachments, colorattachmentsLayouts, depthAttachment, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
    else  subpass = VulkanSubpass(colorAttachments, colorattachmentsLayouts);

    std::vector<VulkanSubpass> subpasses = { subpass };

    renderPass = VulkanRenderPass(attachments, subpasses);

    framebuffer = VulkanFramebuffer(VulkanToolkit::singleton->device, viewport.width, viewport.height, renderPass, attachmentsViews);

    pipeline = VulkanGraphicsPipeline(viewport.width, viewport.height,
        setLayouts, shaderStages, renderPass, foundDepthBuffer, alphaBlending);
}

VulkanRenderStage* VulkanRenderStage::copy()
{
    auto v = new VulkanRenderStage();
    v->setLayouts = setLayouts;
    v->outputImages = outputImages;
    v->shaderStages = shaderStages;
    v->viewport = viewport;
    return v;
}

void VulkanRenderStage::submit(std::vector<VkSemaphore> waitSemaphores)
{
    VkPipelineStageFlags waitStages2[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = waitSemaphores.size();
    submitInfo.pWaitSemaphores = waitSemaphores.data();
    submitInfo.pWaitDstStageMask = waitStages2;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer.handle;

    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &signalSemaphore;

    if (vkQueueSubmit(VulkanToolkit::singleton->mainQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit draw command buffer!");
    }
}
