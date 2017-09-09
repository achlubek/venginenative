#pragma once
class VulkanToolkit;
class VulkanDescriptorSetLayout
{
public:
    VulkanDescriptorSetLayout(VulkanToolkit * vulkan);
    ~VulkanDescriptorSetLayout();

    VulkanToolkit * vulkan;
    int allocationCounter = 0;

    std::vector<VkDescriptorPool> descriptorPools = {};
    VkDescriptorSetLayout layout;

    void addField(uint32_t binding, VkDescriptorType type, VkShaderStageFlags shaderstageflags);
    void compile();

    VulkanDescriptorSet* generateDescriptorSet();
private:
    std::vector<VkDescriptorSetLayoutBinding> bindings;
    void generateNewPool();
};

