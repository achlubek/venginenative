#pragma once 
class VulkanDevice;
#include "Internal/VulkanSingleAllocation.h"
#include <vulkan.h>
enum VulkanBufferType {
    uniform, 
    storage
};
class VulkanGenericBuffer
{
public:
    VulkanGenericBuffer(VulkanDevice * device, VkBufferUsageFlags usage, VkDeviceSize s);
    ~VulkanGenericBuffer();
    void map(VkDeviceSize offset, VkDeviceSize size, void** data);
    void unmap();

    VkBuffer getBuffer();
    uint64_t getSize();

private:
    VulkanDevice * device;
    VkBuffer buffer;
    VulkanSingleAllocation bufferMemory;
    uint64_t size;
    VulkanBufferType type{ VulkanBufferType::uniform };
};
