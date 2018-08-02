#pragma once
class VulkanDevice;
class VulkanRenderStage;
class Object3dInfo;
class VulkanSwapChainOutput
{
public:
    VulkanSwapChainOutput(VulkanDevice* device, VulkanRenderStage* stage);
    ~VulkanSwapChainOutput();
    void beginDrawing();
    void endDrawing();
    void drawMesh(Object3dInfo *info, uint32_t instances);
    void submit(std::vector<VkSemaphore> waitSemaphores);
    std::vector<VkSemaphore> getSignalSemaphores();
private:
    VulkanDevice * device;
    std::vector<VulkanRenderStage*> outputStages;
    VkSemaphore imageAvailableSemaphore;
    uint32_t imageIndex;
    bool firstFrameDrawn = false;
};

