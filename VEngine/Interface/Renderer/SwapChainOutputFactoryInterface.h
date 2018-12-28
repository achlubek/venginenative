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
            SwapChainOutputInterface * build(RenderStageInterface* stage);
        };

    }
}