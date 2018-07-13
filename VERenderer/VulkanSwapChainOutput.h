#pragma once
class VulkanDevice;
class VulkanRenderStage;
class VulkanSwapChainOutput
{
public:
    VulkanSwapChainOutput(VulkanDevice* device, VulkanRenderStage* stage);
    ~VulkanSwapChainOutput();
    void beginDrawing();
    void endDrawing();
    void submit(std::vector<VkSemaphore> waitSemaphores);
private:
    VulkanDevice * device;
    std::vector<VulkanRenderStage*> outputStages;
    VkSemaphore imageAvailableSemaphore;
    uint32_t imageIndex;
};

