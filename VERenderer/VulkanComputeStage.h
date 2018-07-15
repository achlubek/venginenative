#pragma once
class VulkanDevice;
class VulkanDescriptorSet; 
class VulkanCommandBuffer;
class VulkanGraphicsPipeline;
class VulkanShaderModule;
class VulkanDescriptorSetLayout;

class VulkanComputeStage
{
public:
    VulkanComputeStage(VulkanDevice * device,
        VulkanShaderModule* shader,
        std::vector<VulkanDescriptorSetLayout*> layouts);
    ~VulkanComputeStage();

    void beginRecording();
    void endRecording();
    void dispatch(std::vector<VulkanDescriptorSet*> sets, uint32_t groupX, uint32_t groupY, uint32_t groupZ);

    void submit(std::vector<VkSemaphore> waitSemaphores);
    void submitNoSemaphores(std::vector<VkSemaphore> waitSemaphores);
    void compile();
    VkSemaphore getSignalSemaphore();
private:

    VulkanDevice * device;
    VkSemaphore signalSemaphore;
    VulkanCommandBuffer* commandBuffer;
    VulkanGraphicsPipeline* pipeline;

    std::vector<VulkanDescriptorSetLayout*> setLayouts;
    VulkanShaderModule* shader;
};

