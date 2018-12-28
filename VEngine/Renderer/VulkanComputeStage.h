#pragma once
#include "../Interface/Renderer/ComputeStageInterface.h"
#include "../Interface/Renderer/SemaphoreInterface.h"

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
        class DescriptorSetLayout;

        class VulkanComputeStage : public ComputeStageInterface
        {
        public:
            VulkanComputeStage(Internal::VulkanDevice * device,
                VulkanShaderModule* shader,
                std::vector<VulkanDescriptorSetLayout*> layouts);
            ~VulkanComputeStage();

            virtual void beginRecording() override;
            virtual void endRecording() override;
            virtual void dispatch(std::vector<VulkanDescriptorSet*> sets, uint32_t groupX, uint32_t groupY, uint32_t groupZ) override;

            virtual void submit(std::vector<SemaphoreInterface*> waitSemaphores) override;
            virtual void submitNoSemaphores(std::vector<SemaphoreInterface*> waitSemaphores) override;
            virtual SemaphoreInterface* getSignalSemaphore() override;
        private:

            Internal::VulkanDevice * device;
            SemaphoreInterface* signalSemaphore;
            Internal::VulkanCommandBuffer* commandBuffer;
            Internal::VulkanGraphicsPipeline* pipeline;

            std::vector<VulkanDescriptorSetLayout*> setLayouts;
            VulkanShaderModule* shader;
            bool compiled = false;
            void compile();
        };

    }
}