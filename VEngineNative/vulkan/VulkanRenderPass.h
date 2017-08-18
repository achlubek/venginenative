#pragma once
class VulkanRenderPass
{
public:
	VulkanRenderPass() {}
	VulkanRenderPass(std::vector<VulkanAttachment> attachments, std::vector<VulkanSubpass> subpasses);
    ~VulkanRenderPass();
    VkRenderPass handle;
};

