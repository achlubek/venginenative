#pragma once
#include <vector>
#include <cstdint>

namespace VEngine
{
    namespace Renderer
    {
        class DescriptorSetInterface;
        class DescriptorSetLayoutInterface;
        class ShaderModuleInterface;

        class ComputeStageInterface
        {
        public:
            void beginRecording();
            void endRecording();
            void dispatch(std::vector<DescriptorSetInterface*> sets, uint32_t groupX, uint32_t groupY, uint32_t groupZ);

            void submit(std::vector<VkSemaphore> waitSemaphores);
            void submitNoSemaphores(std::vector<VkSemaphore> waitSemaphores);
            VkSemaphore getSignalSemaphore();
        };

    }
}