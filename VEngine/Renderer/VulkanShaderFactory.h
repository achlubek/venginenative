#pragma once
class VulkanDevice;
class VulkanShaderModule;
#include "VulkanEnums.h"

class VulkanShaderFactory
{
public:
    VulkanShaderFactory(VulkanDevice* device);
    ~VulkanShaderFactory();

    VulkanShaderModule* build(VulkanShaderModuleType type, std::string mediakey);
private:
    VulkanDevice * device;
};

