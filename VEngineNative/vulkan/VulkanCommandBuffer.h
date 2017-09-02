#pragma once
class VulkanCommandBuffer
{
public:
    VkCommandBuffer handle;

    VulkanCommandBuffer() {};
    VulkanCommandBuffer(VkCommandBufferLevel level);
    ~VulkanCommandBuffer();

    void begin(VkCommandBufferUsageFlags flags);
    void end();
};
