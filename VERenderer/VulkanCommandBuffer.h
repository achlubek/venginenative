#pragma once
class VulkanToolkit;
class VulkanCommandBuffer
{
public:
    VkCommandBuffer handle;
    VulkanToolkit * vulkan;
     
    VulkanCommandBuffer(VulkanToolkit * vulkan, VkCommandBufferLevel level);
    ~VulkanCommandBuffer();

    void begin(VkCommandBufferUsageFlags flags);
    void end();
};
