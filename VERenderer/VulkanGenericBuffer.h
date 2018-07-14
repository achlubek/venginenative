#pragma once 
class VulkanDevice;
#include "Internal/VulkanSingleAllocation.h"

#include "VulkanEnums.h"

class VulkanGenericBuffer
{
public:
    VulkanGenericBuffer(VulkanDevice * device, VulkanBufferType type, uint64_t s);
    ~VulkanGenericBuffer();
    void map(uint64_t offset, uint64_t size, void** data);
    void unmap();

    VkBuffer getBuffer();
    uint64_t getSize();
    VulkanBufferType getType();

private:
    VulkanDevice * device;
    VkBuffer buffer;
    VulkanSingleAllocation bufferMemory;
    uint64_t size;
    VulkanBufferType type{ VulkanBufferType::BufferTypeUniform };
};
