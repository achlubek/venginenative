#pragma once
class VulkanDevice;

namespace VEngine
{
    namespace Renderer
    {
        namespace Internal
        {
            class VulkanDevice;
        }
        class VulkanSwapChainOutput;
        class VulkanRenderStage;

        class VulkanSwapChainOutputFactory
        {
        public:
            VulkanSwapChainOutputFactory(Internal::VulkanDevice* device);
            ~VulkanSwapChainOutputFactory();

            VulkanSwapChainOutput* build(VulkanRenderStage* stage); // todo in future allow compute shader to draw to swapchain
        private:
            Internal::VulkanDevice * device;
        };

    }
}