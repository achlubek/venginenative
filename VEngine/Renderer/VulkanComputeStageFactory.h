#pragma once
class VulkanDevice;
class VulkanComputeStage;
class VulkanShaderModule;
class VulkanDescriptorSetLayout;
class VulkanComputeStageFactory
{
public:
    VulkanComputeStageFactory(VulkanDevice* device);
    ~VulkanComputeStageFactory();

    VulkanComputeStage* build(VulkanShaderModule* shader, std::vector<VulkanDescriptorSetLayout*> layouts);
private:
    VulkanDevice * device;
};

