#pragma once
class VulkanDevice;
#include <vulkan.h>

enum VulkanShaderModuleType {
    Vertex,
    Fragment,
    Compute
};

class VulkanShaderModule
{
public:
    VulkanShaderModule(VulkanDevice * device, VulkanShaderModuleType type, std::string path);
    ~VulkanShaderModule();
    VkShaderModule getHandle();
    VulkanShaderModuleType getType();

private:
    VkShaderModule handle;
    VulkanDevice * device;
    VulkanShaderModuleType type;
};
