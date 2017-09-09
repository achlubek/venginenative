#pragma once
class VulkanToolkit;
class VulkanShaderModule
{
public:
    VkShaderModule handle;
    VulkanToolkit * vulkan;
    VulkanShaderModule(VulkanToolkit * vulkan, std::string path);
    ~VulkanShaderModule();
    VkPipelineShaderStageCreateInfo createShaderStage(VkShaderStageFlagBits type, const char* entrypoint);
};
