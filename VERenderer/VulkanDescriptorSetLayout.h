#pragma once
class VulkanDevice;
class VulkanDescriptorSet;
#include "VulkanEnums.h" 

class VulkanDescriptorSetLayout
{
public:
    VulkanDescriptorSetLayout(VulkanDevice * device);
    ~VulkanDescriptorSetLayout();

    void addField(VulkanDescriptorSetFieldType fieldType, VulkanDescriptorSetFieldStage fieldAccesibility);
    void compile();

    VulkanDescriptorSet* generateDescriptorSet();
    VkDescriptorSetLayout getHandle();
private:
    std::vector<VkDescriptorSetLayoutBinding> bindings;
    void generateNewPool();

    VulkanDevice * device;
    int allocationCounter = 0;

    std::vector<VkDescriptorPool> descriptorPools = {};
    VkDescriptorSetLayout layout;
};

