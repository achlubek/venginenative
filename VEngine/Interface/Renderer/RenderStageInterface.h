#pragma once
#include <vector>
#include <cstdint>
#include "Enums.h"

namespace VEngine
{
    namespace Renderer
    {
        namespace Internal
        {
        class Object3dInfoInterface;
        class ShaderModuleInterface;
        class DescriptorSetInterface;
        class DescriptorSetLayoutInterface;
        class AttachmentInterface;

        class RenderStageInterface
        {
        public:
            void beginDrawing();
            void endDrawing();
            void setSets(std::vector<DescriptorSetInterface*> sets);
            void setSet(size_t index, DescriptorSetInterface* set);
            void drawMesh(Object3dInfoInterface* info, uint32_t instances);
            void submit(std::vector<VkSemaphore> waitSemaphores);
            void submitNoSemaphores(std::vector<VkSemaphore> waitSemaphores);
            RenderStageInterface* copy();
            RenderStageInterface* copy(std::vector<AttachmentInterface*> outputImages);
            VkSemaphore getSignalSemaphore();
        };

    }
}
