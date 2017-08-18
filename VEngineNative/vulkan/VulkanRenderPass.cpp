#include "stdafx.h" 


VulkanRenderPass::VulkanRenderPass(std::vector<VulkanAttachment> attachments, std::vector<VulkanSubpass> subpasses)
{
	std::vector<VkAttachmentDescription> descs = {};
	descs.resize(attachments.size());
	for (int i = 0; i < attachments.size(); i++)descs[i] = attachments[i].description;

	std::vector<VkSubpassDescription> subpassesraw = {};
	subpassesraw.resize(subpasses.size());
	for (int i = 0; i < subpasses.size(); i++)subpassesraw[i] = subpasses[i].description;

    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = descs.size();
    renderPassInfo.pAttachments = descs.data();
    renderPassInfo.subpassCount = subpassesraw.size();
	renderPassInfo.pSubpasses = subpassesraw.data();

    vkCreateRenderPass(VulkanToolkit::singleton->device, &renderPassInfo, nullptr, &handle);
}


VulkanRenderPass::~VulkanRenderPass()
{
}
