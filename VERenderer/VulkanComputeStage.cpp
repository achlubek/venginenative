#include "stdafx.h"
#include "VulkanComputeStage.h"


VulkanComputeStage::VulkanComputeStage(VulkanToolkit * ivulkan)
    : vulkan(ivulkan)
{
    setLayouts = {};  
}


VulkanComputeStage::~VulkanComputeStage()
{
}

void VulkanComputeStage::addDescriptorSetLayout(VkDescriptorSetLayout lay)
{
    setLayouts.push_back(lay);
}

void VulkanComputeStage::setShaderStage(VkPipelineShaderStageCreateInfo ss)
{
    shader = ss;
}

void VulkanComputeStage::compile()
{
    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    vkCreateSemaphore(vulkan->device, &semaphoreInfo, nullptr, &signalSemaphore);

    commandBuffer = new VulkanCommandBuffer(vulkan, VK_COMMAND_BUFFER_LEVEL_PRIMARY);
     
    pipeline = new VulkanGraphicsPipeline(vulkan, setLayouts, shader);
}


void VulkanComputeStage::beginRecording()
{
    cmdMeshesCounts = 0;
    commandBuffer->begin(VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT);
 
    vkCmdBindPipeline(commandBuffer->handle, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline->graphicsPipeline);

}
void VulkanComputeStage::endRecording()
{
    commandBuffer->end();
}

void VulkanComputeStage::dispatch(std::vector<VulkanDescriptorSet*> sets, uint32_t groupX, uint32_t groupY, uint32_t groupZ)
{
    std::vector<VkDescriptorSet> realsets = {};
    for (int i = 0; i < sets.size(); i++) {
        realsets.push_back(sets[i]->set);
    }
    vkCmdBindDescriptorSets(commandBuffer->handle, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline->pipelineLayout, 0, realsets.size(), realsets.data(), 0, nullptr);
    vkCmdDispatch(commandBuffer->handle, groupX, groupY, groupZ);
}

void VulkanComputeStage::submit(std::vector<VkSemaphore> waitSemaphores)
{
    VkPipelineStageFlags waitStages2[] = { VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT };
    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = waitSemaphores.size();
    submitInfo.pWaitSemaphores = waitSemaphores.data();
    submitInfo.pWaitDstStageMask = waitStages2;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer->handle;

    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &signalSemaphore;

    if (vkQueueSubmit(vulkan->mainQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit draw command buffer!");
    }
}

void VulkanComputeStage::submitNoSemaphores(std::vector<VkSemaphore> waitSemaphores)
{
    VkPipelineStageFlags waitStages2[] = { VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT };
    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = waitSemaphores.size();
    submitInfo.pWaitSemaphores = waitSemaphores.data();
    submitInfo.pWaitDstStageMask = waitStages2;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer->handle;

    submitInfo.signalSemaphoreCount = 0;
    submitInfo.pSignalSemaphores = nullptr;

    if (vkQueueSubmit(vulkan->mainQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit draw command buffer!");
    }
}
