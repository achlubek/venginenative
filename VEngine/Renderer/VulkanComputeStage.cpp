#include "stdafx.h"
#include "Internal/VulkanDevice.h"
#include "VulkanComputeStage.h"
#include "VulkanDescriptorSet.h"
#include "Internal/VulkanCommandBuffer.h"
#include "Internal/VulkanGraphicsPipeline.h"
#include "VulkanShaderModule.h"
#include "VulkanSemaphore.h"
#include "VulkanSemaphoreFactory.h"
#include "VulkanDescriptorSetLayout.h"

namespace VEngine
{
    namespace Renderer
    {
        using namespace VEngine::Renderer::Internal;

        VulkanComputeStage::VulkanComputeStage(VulkanDevice * device,
            SemaphoreFactoryInterface* semaphoreFactory,
            ShaderModuleInterface* shader,
            std::vector<DescriptorSetLayoutInterface*> layouts)
            : device(device), semaphoreFactory(semaphoreFactory), shader(shader), setLayouts(layouts)
        {
        }


        VulkanComputeStage::~VulkanComputeStage()
        {
            safedelete(signalSemaphore);
            safedelete(pipeline);
            safedelete(commandBuffer);
        }

        void VulkanComputeStage::compile()
        {
            signalSemaphore = static_cast<VulkanSemaphore*>(semaphoreFactory->build());

            commandBuffer = new VulkanCommandBuffer(device, VK_COMMAND_BUFFER_LEVEL_PRIMARY);

            VkPipelineShaderStageCreateInfo info = {};
            info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;

            auto type = VK_SHADER_STAGE_VERTEX_BIT;
            if (shader->getType() == VEngineShaderModuleType::Fragment) {
                type = VK_SHADER_STAGE_FRAGMENT_BIT;
            }
            if (shader->getType() == VEngineShaderModuleType::Compute) {
                type = VK_SHADER_STAGE_COMPUTE_BIT;
            }

            info.stage = type;
            info.module = static_cast<VulkanShaderModule*>(shader)->getHandle();
            info.pName = "main";

            std::vector<VulkanDescriptorSetLayout*> vulkanSetLayouts = {};
            for (auto &setLayout : setLayouts) {
                vulkanSetLayouts.push_back(static_cast<VulkanDescriptorSetLayout*>(setLayout));
            }

            pipeline = new VulkanGraphicsPipeline(device, vulkanSetLayouts, info);
            compiled = true;
        }

        SemaphoreInterface* VulkanComputeStage::getSignalSemaphore()
        {
            if (!compiled) {
                compile();
            }
            return signalSemaphore;
        }


        void VulkanComputeStage::beginRecording()
        {
            if (!compiled) {
                compile();
            }
            commandBuffer->begin(VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT);
            vkCmdBindPipeline(commandBuffer->getHandle(), VK_PIPELINE_BIND_POINT_COMPUTE, pipeline->getPipeline());

        }
        void VulkanComputeStage::endRecording()
        {
            commandBuffer->end();
        }

        void VulkanComputeStage::dispatch(std::vector<DescriptorSetInterface*> sets, uint32_t groupX, uint32_t groupY, uint32_t groupZ)
        {
            std::vector<VkDescriptorSet> realsets = {};
            for (int i = 0; i < sets.size(); i++) {
                realsets.push_back(static_cast<VulkanDescriptorSet*>(sets[i])->getSet());
            }
            vkCmdBindDescriptorSets(commandBuffer->getHandle(), VK_PIPELINE_BIND_POINT_COMPUTE, pipeline->getPipelineLayout(), 0u, static_cast<uint32_t>(realsets.size()), realsets.data(), 0u, nullptr);
            vkCmdDispatch(commandBuffer->getHandle(), groupX, groupY, groupZ);
        }

        void VulkanComputeStage::submit(std::vector<SemaphoreInterface*> waitSemaphores)
        {
            std::vector<VkPipelineStageFlags> stageFlags = {};
            std::vector<VkSemaphore> vulkanSemaphores = {};
            for (int i = 0; i < waitSemaphores.size(); i++) {
                stageFlags.push_back(
                    VK_PIPELINE_STAGE_ALL_COMMANDS_BIT |
                    VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT);
                vulkanSemaphores.push_back(static_cast<VulkanSemaphore*>(waitSemaphores[i])->getHandle());
            }
            VkSubmitInfo submitInfo = {};
            submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            submitInfo.waitSemaphoreCount = static_cast<uint32_t>(vulkanSemaphores.size());
            submitInfo.pWaitSemaphores = vulkanSemaphores.data();
            submitInfo.pWaitDstStageMask = stageFlags.data();

            submitInfo.commandBufferCount = 1;
            auto cbufferHandle = commandBuffer->getHandle();
            submitInfo.pCommandBuffers = &cbufferHandle;

            submitInfo.signalSemaphoreCount = 1;
            submitInfo.pSignalSemaphores = static_cast<VulkanSemaphore*>(signalSemaphore)->getHandlePointer();

            if (vkQueueSubmit(device->getMainQueue(), 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
                throw std::runtime_error("failed to submit draw command buffer!");
            }
        }

        void VulkanComputeStage::submitNoSemaphores(std::vector<SemaphoreInterface*> waitSemaphores)
        {
            std::vector<VkPipelineStageFlags> stageFlags = {};
            std::vector<VkSemaphore> vulkanSemaphores = {};
            for (int i = 0; i < waitSemaphores.size(); i++) {
                stageFlags.push_back(
                    VK_PIPELINE_STAGE_ALL_COMMANDS_BIT | 
                    VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT);
                vulkanSemaphores.push_back(static_cast<VulkanSemaphore*>(waitSemaphores[i])->getHandle());
            }
            VkSubmitInfo submitInfo = {};
            submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            submitInfo.waitSemaphoreCount = static_cast<uint32_t>(vulkanSemaphores.size());
            submitInfo.pWaitSemaphores = vulkanSemaphores.data();
            submitInfo.pWaitDstStageMask = stageFlags.data();

            submitInfo.commandBufferCount = 1;
            auto cbufferHandle = commandBuffer->getHandle();
            submitInfo.pCommandBuffers = &cbufferHandle;

            submitInfo.signalSemaphoreCount = 0;
            submitInfo.pSignalSemaphores = nullptr;

            if (vkQueueSubmit(device->getMainQueue(), 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
                throw std::runtime_error("failed to submit draw command buffer!");
            }
        }

    }
}