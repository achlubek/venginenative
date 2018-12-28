#pragma once
#include <cstdint>

namespace VEngine
{
    namespace Renderer
    {
        class RenderStageInterface;
        class Object3dInfoInterface;

        class SwapChainOutputInterface
        {
        public:
            void beginDrawing();
            void endDrawing();
            void drawMesh(Object3dInfoInterface *info, uint32_t instances);
            void submit(std::vector<VkSemaphore> waitSemaphores);
            std::vector<VkSemaphore> getSignalSemaphores();
        };

    }
}