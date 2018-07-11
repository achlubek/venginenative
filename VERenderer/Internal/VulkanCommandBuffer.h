#pragma once
class VulkanDevice;
class VulkanCommandBuffer
{
public:
    VulkanCommandBuffer(VulkanDevice * device, VkCommandBufferLevel level);
    ~VulkanCommandBuffer();
    void begin(VkCommandBufferUsageFlags flags);
    void end();

private:
    VkCommandBuffer handle;
    VulkanDevice * device;
};
