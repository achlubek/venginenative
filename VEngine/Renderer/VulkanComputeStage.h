#pragma once

namespace VEngine
{
    namespace Renderer
    {
        namespace Internal
        {
            class VulkanDevice;
            class VulkanCommandBuffer;
            class VulkanGraphicsPipeline;
        }
        class VulkanDescriptorSet;
        class VulkanDescriptorSetLayout;
        class VulkanShaderModule;

        class VulkanComputeStage
        {
        public:
            VulkanComputeStage(Internal::VulkanDevice * device,
                VulkanShaderModule* shader,
                std::vector<VulkanDescriptorSetLayout*> layouts);
            ~VulkanComputeStage();

            void beginRecording();
            void endRecording();
            void dispatch(std::vector<VulkanDescriptorSet*> sets, uint32_t groupX, uint32_t groupY, uint32_t groupZ);

            void submit(std::vector<VkSemaphore> waitSemaphores);
            void submitNoSemaphores(std::vector<VkSemaphore> waitSemaphores);
            VkSemaphore getSignalSemaphore();
        private:

            Internal::VulkanDevice * device;
            VkSemaphore signalSemaphore;
            Internal::VulkanCommandBuffer* commandBuffer;
            Internal::VulkanGraphicsPipeline* pipeline;

            std::vector<VulkanDescriptorSetLayout*> setLayouts;
            VulkanShaderModule* shader;
            bool compiled = false;
            void compile();
        };

    }
}