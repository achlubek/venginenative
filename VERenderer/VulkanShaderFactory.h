#pragma once
class VulkanToolkit;
#include "VulkanShaderModule.h"
class VulkanShaderFactory
{
public:
    VulkanShaderFactory(VulkanToolkit* vulkan);
    ~VulkanShaderFactory();

    VulkanShaderModule* build(VulkanShaderModuleType type, std::string mediakey);
private:
    VulkanToolkit * vulkan;
};

