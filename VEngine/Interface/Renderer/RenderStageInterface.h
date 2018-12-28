#pragma once
#include <vector>
#include <cstdint>
#include "Enums.h"

namespace VEngine
{
    namespace Renderer
    {
        class Object3dInfoInterface;
        class ShaderModuleInterface;
        class DescriptorSetInterface;
        class DescriptorSetLayoutInterface;
        class AttachmentInterface;
        class SemaphoreInterface;

        class RenderStageInterface
        {
        public:
            virtual void beginDrawing() = 0;
            virtual void endDrawing() = 0;
            virtual void setSets(std::vector<DescriptorSetInterface*> sets) = 0;
            virtual void setSet(size_t index, DescriptorSetInterface* set) = 0;
            virtual void drawMesh(Object3dInfoInterface* info, uint32_t instances) = 0;
            virtual void submit(std::vector<SemaphoreInterface*> waitSemaphores) = 0;
            virtual void submitNoSemaphores(std::vector<SemaphoreInterface*> waitSemaphores) = 0;
            virtual RenderStageInterface* copy() = 0;
            virtual RenderStageInterface* copy(std::vector<AttachmentInterface*> outputImages) = 0;
            virtual SemaphoreInterface* getSignalSemaphore() = 0;
        };

    }
}
