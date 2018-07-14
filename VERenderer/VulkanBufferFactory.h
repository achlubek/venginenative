#pragma once
class VulkanDevice;
class VulkanGenericBuffer;
#include "VulkanEnums.h"
class VulkanBufferFactory
{
public:
    VulkanBufferFactory(VulkanDevice* device);
    ~VulkanBufferFactory();

    VulkanGenericBuffer* build(VulkanBufferType type, uint64_t size);
private:
    VulkanDevice * device;
};

