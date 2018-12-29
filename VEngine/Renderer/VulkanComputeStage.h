#pragma once
#include "../Interface/Renderer/ComputeStageInterface.h"

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
        class SemaphoreInterface;
        class SemaphoreFactoryInterface;
        class ShaderModuleInterface;
        class DescriptorSetLayoutInterface;

        class VulkanComputeStage : public ComputeStageInterface
        {
        public:
            VulkanComputeStage(Internal::VulkanDevice * device,
                SemaphoreFactoryInterface* semaphoreFactory,
                ShaderModuleInterface* shader,
                std::vector<DescriptorSetLayoutInterface*> layouts);
            ~VulkanComputeStage();

            virtual void beginRecording() override;
            virtual void endRecording() override;
            virtual void dispatch(std::vector<DescriptorSetInterface*> sets, uint32_t groupX, uint32_t groupY, uint32_t groupZ) override;

            virtual void submit(std::vector<SemaphoreInterface*> waitSemaphores) override;
            virtual void submitNoSemaphores(std::vector<SemaphoreInterface*> waitSemaphores) override;
            virtual SemaphoreInterface* getSignalSemaphore() override;
        private:

            Internal::VulkanDevice * device;
            SemaphoreInterface* signalSemaphore;
            SemaphoreFactoryInterface* semaphoreFactory;
            Internal::VulkanCommandBuffer* commandBuffer;
            Internal::VulkanGraphicsPipeline* pipeline;

            std::vector<DescriptorSetLayoutInterface*> setLayouts;
            ShaderModuleInterface* shader;
            bool compiled = false;
            void compile();
        };

    }
}