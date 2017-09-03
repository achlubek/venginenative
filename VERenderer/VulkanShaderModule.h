#pragma once
class VulkanShaderModule
{
public:
    VkShaderModule handle;
    VulkanShaderModule(std::string path);
    ~VulkanShaderModule();
    VkPipelineShaderStageCreateInfo createShaderStage(VkShaderStageFlagBits type, const char* entrypoint);
};
