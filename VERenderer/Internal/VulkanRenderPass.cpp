#include "stdafx.h" 
#include "VulkanRenderPass.h" 
#include <vulkan.h>


VulkanRenderPass::VulkanRenderPass(VulkanToolkit * ivulkan, std::vector<VulkanAttachment> iattachments, std::vector<VulkanSubpass> subpasses)
    : vulkan(ivulkan)
{
    attachments = iattachments;
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

    vkCreateRenderPass(vulkan->device, &renderPassInfo, nullptr, &handle);
}


VulkanRenderPass::~VulkanRenderPass()
{
    vkDestroyRenderPass(vulkan->device, handle, nullptr);
}
