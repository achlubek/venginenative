#pragma once
class VulkanDevice;
class VulkanRenderStage;
class VulkanShaderModule;
class VulkanDescriptorSetLayout;
class VulkanAttachment;
class VulkanRenderStageFactory
{
public:
    VulkanRenderStageFactory(VulkanDevice* device);
    ~VulkanRenderStageFactory();
    VulkanRenderStage* build(int width, int height,
        std::vector<VulkanShaderModule*> shaders,
        std::vector<VulkanDescriptorSetLayout*> layouts,
        std::vector<VulkanAttachment*> outputImages);

private:
    VulkanDevice * device;
};

