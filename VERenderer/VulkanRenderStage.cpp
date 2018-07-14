#include "stdafx.h"
#include "VulkanRenderStage.h"
#include "Object3dInfo.h"
#include "VulkanDescriptorSetLayout.h"
#include "VulkanDescriptorSet.h"
#include "Object3dInfo.h"
#include "Internal/VulkanCommandBuffer.h"
#include "Internal/VulkanRenderPass.h"
#include "Internal/VulkanSubpass.h"
#include "Internal/VulkanGraphicsPipeline.h"
#include "Internal/VulkanVertexBuffer.h"
#include "Internal/VulkanFramebuffer.h"
#include "VulkanAttachment.h"
#include "VulkanImage.h"
#include "Internal/VulkanDevice.h"
#include "VulkanShaderModule.h"

VulkanRenderStage::VulkanRenderStage(VulkanDevice * device, int width, int height,
    std::vector<VulkanShaderModule*> shaders,
    std::vector<VulkanDescriptorSetLayout*> layouts,
    std::vector<VulkanAttachment*> outputImages)
    : device(device), width(width), height(height), setLayouts(layouts), outputImages(outputImages), shaders(shaders)
{
    sets = {};
}


#define safedelete(a) if(a!=nullptr){delete a;a=nullptr;}
VulkanRenderStage::~VulkanRenderStage()
{
    //vkDestroySemaphore(vulkan->device, signalSemaphore, nullptr);
    safedelete(renderPass);
    safedelete(framebuffer);

    safedelete(pipeline);
    safedelete(commandBuffer);
}

void VulkanRenderStage::beginDrawing()
{
    cmdMeshesCounts = 0;
    commandBuffer->begin(VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT);

    VkRenderPassBeginInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass->getHandle();
    renderPassInfo.framebuffer = framebuffer->getHandle();
    renderPassInfo.renderArea.offset = { 0, 0 };
    VkExtent2D extent = {};
    extent.width = width;
    extent.height = height;
    renderPassInfo.renderArea.extent = extent;
    std::vector<VkClearValue> clearValues = {};
    
    for (int i = 0; i < renderPass->getAttachments().size(); i++) {
        if (renderPass->getAttachments()[i]->isCleared()) {
            clearValues.push_back(VkClearValue());
            if (renderPass->getAttachments()[i]->getImage()->isDepthBuffer()) {
                clearValues[clearValues.size() - 1].depthStencil = { 1.0f, 0 };
            }
            else {
                clearValues[clearValues.size() - 1].color = renderPass->getAttachments()[i]->getClearColor();
            }
        }
    }

    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();


    vkCmdBeginRenderPass(commandBuffer->getHandle(), &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(commandBuffer->getHandle(), VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->getPipeline());

}
void VulkanRenderStage::endDrawing()
{
    vkCmdEndRenderPass(commandBuffer->getHandle());
    commandBuffer->end();
}

void VulkanRenderStage::setSets(std::vector<VulkanDescriptorSet*> isets)
{
    sets = isets;
}

void VulkanRenderStage::drawMesh(Object3dInfo * info, size_t instances)
{
    info->getVertexBuffer()->drawInstanced(pipeline, sets, commandBuffer, instances);
    cmdMeshesCounts++;
}

void VulkanRenderStage::compile()
{
    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    vkCreateSemaphore(device->getDevice(), &semaphoreInfo, nullptr, &signalSemaphore);

    commandBuffer = new VulkanCommandBuffer(device, VK_COMMAND_BUFFER_LEVEL_PRIMARY);

    std::vector<VulkanAttachment*> attachments = {};
    std::vector<VkImageView> attachmentsViews = {};
    std::vector<VulkanAttachment*> colorAttachments = { };
    VulkanAttachment* depthAttachment = nullptr;
    std::vector<VkImageLayout> colorattachmentsLayouts = { };

    bool foundDepthBuffer = false;
    for (int i = 0; i < outputImages.size(); i++) {
        auto atta = outputImages[i];
        auto image = atta->getImage();
        auto view = image->getImageView();
        if (image->isDepthBuffer()) {
            depthAttachment = atta;
            foundDepthBuffer = true;
        }
        else {
            colorAttachments.push_back(atta);
            colorattachmentsLayouts.push_back(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
        }
        attachments.push_back(atta);
        attachmentsViews.push_back(view);
    }

    VulkanSubpass subpass;
    if (foundDepthBuffer) subpass = VulkanSubpass(colorAttachments, colorattachmentsLayouts, depthAttachment, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
    else  subpass = VulkanSubpass(colorAttachments, colorattachmentsLayouts);

    std::vector<VulkanSubpass> subpasses = { subpass };

    renderPass = new VulkanRenderPass(device, attachments, subpasses);

    framebuffer = new VulkanFramebuffer(device, width, height, renderPass, attachmentsViews);

    std::vector<VkPipelineShaderStageCreateInfo> shaderStages = {};
    for (int i = 0; i < shaders.size(); i++) {
        auto shader = shaders[i];
        VkPipelineShaderStageCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;

        auto type = VK_SHADER_STAGE_VERTEX_BIT;
        if (shader->getType() == VulkanShaderModuleType::Fragment) {
            type = VK_SHADER_STAGE_FRAGMENT_BIT;
        }
        if (shader->getType() == VulkanShaderModuleType::Compute) {
            type = VK_SHADER_STAGE_COMPUTE_BIT;
        }

        info.stage = type;
        info.module = shader->getHandle();
        info.pName = "main";
        shaderStages.push_back(info);
    }

    pipeline = new VulkanGraphicsPipeline(device, width, height,
        setLayouts, shaderStages, renderPass, foundDepthBuffer, topology, cullFlags);
}

VulkanRenderStage* VulkanRenderStage::copy()
{
    auto v = new VulkanRenderStage(device, width, height, shaders, setLayouts, outputImages);
    v->setSets(sets);
    return v;
}

VulkanRenderStage * VulkanRenderStage::copy(std::vector<VulkanAttachment*> ioutputImages)
{
    auto v = new VulkanRenderStage(device, width, height, shaders, setLayouts, ioutputImages);
    v->setSets(sets);
    return v;
}

VkSemaphore VulkanRenderStage::getSignalSemaphore()
{
    return signalSemaphore;
}

void VulkanRenderStage::submit(std::vector<VkSemaphore> waitSemaphores)
{
    vkDeviceWaitIdle(device->getDevice());
    VkPipelineStageFlags waitStages2[] = { VK_PIPELINE_STAGE_ALL_COMMANDS_BIT };
    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = waitSemaphores.size();
    submitInfo.pWaitSemaphores = waitSemaphores.data();
    submitInfo.pWaitDstStageMask = waitStages2;

    submitInfo.commandBufferCount = 1;
    auto cbufferHandle = commandBuffer->getHandle();
    submitInfo.pCommandBuffers = &cbufferHandle;

    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &signalSemaphore;

    if (vkQueueSubmit(device->getMainQueue(), 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit draw command buffer!");
    }
}

void VulkanRenderStage::submitNoSemaphores(std::vector<VkSemaphore> waitSemaphores)
{
    VkPipelineStageFlags waitStages2[] = { VK_PIPELINE_STAGE_ALL_COMMANDS_BIT };
    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = waitSemaphores.size();
    submitInfo.pWaitSemaphores = waitSemaphores.data();
    submitInfo.pWaitDstStageMask = waitStages2;

    submitInfo.commandBufferCount = 1;
    auto cbufferHandle = commandBuffer->getHandle();
    submitInfo.pCommandBuffers = &cbufferHandle;

    submitInfo.signalSemaphoreCount = 0;
    submitInfo.pSignalSemaphores = nullptr;

    if (vkQueueSubmit(device->getMainQueue(), 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit draw command buffer!");
    }
}