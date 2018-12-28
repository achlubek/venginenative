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
        class SemaphoreInterface;

        class ComputeStageInterface
        {
        public:
            virtual void beginRecording() = 0;
            virtual void endRecording() = 0;
            virtual void dispatch(std::vector<DescriptorSetInterface*> sets, uint32_t groupX, uint32_t groupY, uint32_t groupZ) = 0;
            virtual void submit(std::vector<SemaphoreInterface*> waitSemaphores) = 0;
            virtual void submitNoSemaphores(std::vector<SemaphoreInterface*> waitSemaphores) = 0;
            virtual SemaphoreInterface* getSignalSemaphore() = 0;
        };

    }
}