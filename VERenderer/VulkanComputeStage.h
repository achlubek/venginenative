#pragma once
class VulkanToolkit;
class VulkanComputeStage
{
public:
    VulkanComputeStage(VulkanToolkit * vulkan);
    ~VulkanComputeStage();

    VulkanToolkit * vulkan;
    VkSemaphore signalSemaphore;
    VulkanCommandBuffer* commandBuffer;
    VulkanGraphicsPipeline* pipeline;
    std::vector<VkDescriptorSetLayout> setLayouts;
    VkPipelineShaderStageCreateInfo shader;

    void addDescriptorSetLayout(VkDescriptorSetLayout lay);
    void setShaderStage(VkPipelineShaderStageCreateInfo ss);

    int cmdMeshesCounts = 0;
    void beginRecording();
    void endRecording();
    void dispatch(std::vector<VulkanDescriptorSet*> sets, uint32_t groupX, uint32_t groupY, uint32_t groupZ);
    void compile(); 

    void submit(std::vector<VkSemaphore> waitSemaphores);
    void submitNoSemaphores(std::vector<VkSemaphore> waitSemaphores);

};

