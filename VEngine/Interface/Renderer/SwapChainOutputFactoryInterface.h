#pragma once
class VulkanDevice;

namespace VEngine
{
    namespace Renderer
    {
        class SwapChainOutputInterface;
        class RenderStageInterface;

        class SwapChainOutputFactoryInterface
        {
        public:
            virtual SwapChainOutputInterface * build(RenderStageInterface* stage) = 0;
        };

    }
}