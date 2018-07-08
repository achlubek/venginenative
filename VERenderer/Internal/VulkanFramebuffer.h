#pragma once
class VulkanToolkit;
class VulkanFramebuffer
{
public:
    VkFramebuffer handle;
    VulkanToolkit * vulkan;
    VulkanFramebuffer(VulkanToolkit * vulkan, uint32_t width, uint32_t height, VulkanRenderPass* renderPass, std::vector<VkImageView> &attachments);
    ~VulkanFramebuffer();
};
