#pragma once
class VulkanFramebuffer
{
public:
    VkFramebuffer handle;
    VulkanFramebuffer() {}
    VulkanFramebuffer(VkDevice device, uint32_t width, uint32_t height, VulkanRenderPass renderPass, std::vector<VkImageView> &attachments);
    ~VulkanFramebuffer();
};
