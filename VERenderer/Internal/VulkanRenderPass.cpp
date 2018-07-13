#include "stdafx.h" 
#include "VulkanRenderPass.h" 
#include "VulkanDevice.h" 
#include "VulkanSubpass.h" 
#include "VulkanAttachment.h" 


VulkanRenderPass::VulkanRenderPass(VulkanDevice * device, std::vector<VulkanAttachment*> iattachments, std::vector<VulkanSubpass> subpasses)
    : device(device)
{
    attachments = iattachments;
    std::vector<VkAttachmentDescription> descs = {};
    descs.resize(attachments.size());
    for (int i = 0; i < attachments.size(); i++)descs[i] = attachments[i]->getDescription();

    std::vector<VkSubpassDescription> subpassesraw = {};
    subpassesraw.resize(subpasses.size());
    for (int i = 0; i < subpasses.size(); i++)subpassesraw[i] = subpasses[i].getHandle();

    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = descs.size();
    renderPassInfo.pAttachments = descs.data();
    renderPassInfo.subpassCount = subpassesraw.size();
    renderPassInfo.pSubpasses = subpassesraw.data();

    vkCreateRenderPass(device->getDevice(), &renderPassInfo, nullptr, &handle);
}


VulkanRenderPass::~VulkanRenderPass()
{
    vkDestroyRenderPass(device->getDevice(), handle, nullptr);
}

VkRenderPass VulkanRenderPass::getHandle()
{
    return handle;
}

std::vector<VulkanAttachment*> VulkanRenderPass::getAttachments()
{
    return attachments;
}
