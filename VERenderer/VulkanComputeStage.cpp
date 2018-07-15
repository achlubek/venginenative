#include "stdafx.h"
#include "Internal/VulkanDevice.h"
#include "VulkanComputeStage.h"
#include "VulkanDescriptorSet.h"
#include "Internal/VulkanCommandBuffer.h"
#include "Internal/VulkanGraphicsPipeline.h"

#include "VulkanShaderModule.h"

VulkanComputeStage::VulkanComputeStage(VulkanDevice * device,
    VulkanShaderModule* shader,
    std::vector<VulkanDescriptorSetLayout*> layouts)
    : device(device), shader(shader), setLayouts(layouts)
{
}


VulkanComputeStage::~VulkanComputeStage()
{
}

void VulkanComputeStage::compile()
{
    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    vkCreateSemaphore(device->getDevice(), &semaphoreInfo, nullptr, &signalSemaphore);

    commandBuffer = new VulkanCommandBuffer(device, VK_COMMAND_BUFFER_LEVEL_PRIMARY);
     
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

    pipeline = new VulkanGraphicsPipeline(device, setLayouts, info);
}


void VulkanComputeStage::beginRecording()
{
    commandBuffer->begin(VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT);
    vkCmdBindPipeline(commandBuffer->getHandle(), VK_PIPELINE_BIND_POINT_COMPUTE, pipeline->getPipeline());

}
void VulkanComputeStage::endRecording()
{
    commandBuffer->end();
}

void VulkanComputeStage::dispatch(std::vector<VulkanDescriptorSet*> sets, uint32_t groupX, uint32_t groupY, uint32_t groupZ)
{
    std::vector<VkDescriptorSet> realsets = {};
    for (int i = 0; i < sets.size(); i++) {
        realsets.push_back(sets[i]->getSet());
    }
    vkCmdBindDescriptorSets(commandBuffer->getHandle(), VK_PIPELINE_BIND_POINT_COMPUTE, pipeline->getPipelineLayout(), 0u, static_cast<uint32_t>(realsets.size()), realsets.data(), 0u, nullptr);
    vkCmdDispatch(commandBuffer->getHandle(), groupX, groupY, groupZ);
}

void VulkanComputeStage::submit(std::vector<VkSemaphore> waitSemaphores)
{
    VkPipelineStageFlags waitStages2[] = { VK_PIPELINE_STAGE_VERTEX_INPUT_BIT | VK_PIPELINE_STAGE_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
        VK_PIPELINE_STAGE_VERTEX_INPUT_BIT | VK_PIPELINE_STAGE_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT };
    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = static_cast<uint32_t>(waitSemaphores.size());
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

void VulkanComputeStage::submitNoSemaphores(std::vector<VkSemaphore> waitSemaphores)
{
    VkPipelineStageFlags waitStages2[] = { VK_PIPELINE_STAGE_VERTEX_INPUT_BIT | VK_PIPELINE_STAGE_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
        VK_PIPELINE_STAGE_VERTEX_INPUT_BIT | VK_PIPELINE_STAGE_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT };
    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = static_cast<uint32_t>(waitSemaphores.size());
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
