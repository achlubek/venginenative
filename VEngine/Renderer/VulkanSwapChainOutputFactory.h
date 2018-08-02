#pragma once
class VulkanDevice;
class VulkanSwapChainOutput;
class VulkanRenderStage;
class VulkanSwapChainOutputFactory
{
public:
    VulkanSwapChainOutputFactory(VulkanDevice* device);
    ~VulkanSwapChainOutputFactory();

    VulkanSwapChainOutput* build(VulkanRenderStage* stage); // todo in future allow compute shader to draw to swapchain
private:
    VulkanDevice * device;
};

