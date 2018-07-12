#pragma once
class VulkanDevice;
class VulkanDescriptorSet; 
class VulkanCommandBuffer;
class VulkanGraphicsPipeline;
#include <vulkan.h>

class VulkanComputeStage
{
public:
    VulkanComputeStage(VulkanDevice * device);
    ~VulkanComputeStage();

    void beginRecording();
    void endRecording();
    void dispatch(std::vector<VulkanDescriptorSet*> sets, uint32_t groupX, uint32_t groupY, uint32_t groupZ);

    void submit(std::vector<VkSemaphore> waitSemaphores);
    void submitNoSemaphores(std::vector<VkSemaphore> waitSemaphores);
private:

    VulkanDevice * device;
    VkSemaphore signalSemaphore;
    VulkanCommandBuffer* commandBuffer;
    VulkanGraphicsPipeline* pipeline;
    std::vector<VkDescriptorSetLayout> setLayouts;
    VkPipelineShaderStageCreateInfo shader; 

    void addDescriptorSetLayout(VkDescriptorSetLayout lay);
    void setShaderStage(VkPipelineShaderStageCreateInfo ss);

    int cmdMeshesCounts = 0;
    void compile(); 


};

