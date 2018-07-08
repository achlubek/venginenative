#pragma once
class VulkanToolkit;
class VkShaderModule;

enum VulkanShaderModuleType {
    Vertex,
    Fragment,
    Compute
};

class VulkanShaderModule
{
public:
    VulkanShaderModule(VulkanToolkit * vulkan, VulkanShaderModuleType type, std::string path);
    ~VulkanShaderModule();
    VkShaderModule getHandle();
    VulkanShaderModuleType getType();

private:
    VkShaderModule handle;
    VulkanToolkit * vulkan;
    VulkanShaderModuleType type;
};
