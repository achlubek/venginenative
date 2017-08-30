#pragma once
class VulkanRenderPass
{
public:
	VulkanRenderPass() {}
	VulkanRenderPass(std::vector<VulkanAttachment> iattachments, std::vector<VulkanSubpass> subpasses);
    ~VulkanRenderPass();
    VkRenderPass handle;
	std::vector<VulkanAttachment> attachments;
};

