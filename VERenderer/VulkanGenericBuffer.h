#pragma once 
class VulkanToolkit;
#include "VulkanMemoryChunk.h"
class VulkanGenericBuffer
{
public:
    VulkanToolkit * vulkan;
    VkBuffer buffer;
    VulkanSingleAllocation bufferMemory;
    VkDeviceSize size; 
    VulkanGenericBuffer(VulkanToolkit * vulkan, VkBufferUsageFlags usage, VkDeviceSize s);
    ~VulkanGenericBuffer();
    void map(VkDeviceSize offset, VkDeviceSize size, void** data);
    void unmap();
};
