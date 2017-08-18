#pragma once 
class VulkanGenericBuffer
{
public:
    VkBuffer buffer;
    VkDeviceMemory bufferMemory;
	VkDeviceSize size;
	VulkanGenericBuffer();
	VulkanGenericBuffer(VkDeviceSize s);
    ~VulkanGenericBuffer();
    void map(VkDeviceSize offset, VkDeviceSize size, void** data);
    void unmap();
};
