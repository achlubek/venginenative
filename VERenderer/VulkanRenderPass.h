#pragma once
class VulkanToolkit;
class VulkanRenderPass
{
public: 
    VulkanToolkit * vulkan;

    VulkanRenderPass(VulkanToolkit * vulkan, std::vector<VulkanAttachment> iattachments, std::vector<VulkanSubpass> subpasses);
    ~VulkanRenderPass();
    VkRenderPass handle;
    std::vector<VulkanAttachment> attachments;
};

