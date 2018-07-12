#pragma once
class VulkanDevice;
class VulkanDescriptorSet;
class VulkanDescriptorSetLayout
{
public:
    VulkanDescriptorSetLayout(VulkanDevice * device);
    ~VulkanDescriptorSetLayout();

    VulkanDevice * device;
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

