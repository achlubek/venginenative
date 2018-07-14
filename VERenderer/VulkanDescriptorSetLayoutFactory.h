#pragma once
class VulkanDevice;
class VulkanDescriptorSetLayout;
class VulkanDescriptorSetLayoutFactory
{
public:
    VulkanDescriptorSetLayoutFactory(VulkanDevice * device);
    ~VulkanDescriptorSetLayoutFactory();

    VulkanDescriptorSetLayout* build();

private:
    VulkanDevice * device;
};

