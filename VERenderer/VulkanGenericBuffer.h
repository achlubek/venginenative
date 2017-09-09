#pragma once 
class VulkanToolkit;
class VulkanGenericBuffer
{
public:
    VulkanToolkit * vulkan;
    VkBuffer buffer;
    VkDeviceMemory bufferMemory;
    VkDeviceSize size; 
    VulkanGenericBuffer(VulkanToolkit * vulkan, VkBufferUsageFlags usage, VkDeviceSize s);
    ~VulkanGenericBuffer();
    void map(VkDeviceSize offset, VkDeviceSize size, void** data);
    void unmap();
};
