#pragma once 
class VulkanToolkit;
#include "VulkanSingleAllocation.h"
enum VulkanBufferType {
    uniform, 
    storage
};
class VulkanGenericBuffer
{
public:
    VulkanToolkit * vulkan;
    VkBuffer buffer;
    VulkanSingleAllocation bufferMemory;
    VkDeviceSize size; 
    VulkanBufferType type{ VulkanBufferType ::uniform };
    VulkanGenericBuffer(VulkanToolkit * vulkan, VkBufferUsageFlags usage, VkDeviceSize s);
    ~VulkanGenericBuffer();
    void map(VkDeviceSize offset, VkDeviceSize size, void** data);
    void unmap();
};
