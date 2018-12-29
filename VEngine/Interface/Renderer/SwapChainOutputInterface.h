#pragma once
#include <cstdint>

namespace VEngine
{
    namespace Renderer
    {
        class RenderStageInterface;
        class Object3dInfoInterface;
        class SemaphoreInterface;

        class SwapChainOutputInterface
        {
        public:
            virtual void beginDrawing() = 0;
            virtual void endDrawing() = 0;
            virtual void drawMesh(Object3dInfoInterface* info, uint32_t instances) = 0;
            virtual void submit(std::vector<SemaphoreInterface*> waitSemaphores) = 0;
            virtual std::vector<SemaphoreInterface*> getSignalSemaphores() = 0;
        };

    }
}