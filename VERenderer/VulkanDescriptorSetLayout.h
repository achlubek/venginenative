#pragma once
class VulkanDescriptorSetLayout
{
public:
    VulkanDescriptorSetLayout();
    ~VulkanDescriptorSetLayout();

    int allocationCounter = 0;

    std::vector<VkDescriptorPool> descriptorPools = {};
    VkDescriptorSetLayout layout;

    void addField(uint32_t binding, VkDescriptorType type, VkShaderStageFlags shaderstageflags);
    void compile();

    VulkanDescriptorSet generateDescriptorSet();
private:
    std::vector<VkDescriptorSetLayoutBinding> bindings;
    void generateNewPool();
};

