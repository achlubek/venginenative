#pragma once
class VulkanSubpass
{
public:
	VulkanSubpass() {}
	VulkanSubpass(std::vector<VulkanAttachment> colorattachments, std::vector<VkImageLayout> colorattachmentlayouts, VulkanAttachment depthstencilattachment, VkImageLayout despthattachmentlayout);
	VulkanSubpass(std::vector<VulkanAttachment> colorattachments, std::vector<VkImageLayout> colorattachmentlayouts);
	VkSubpassDescription description;
	~VulkanSubpass();
};

