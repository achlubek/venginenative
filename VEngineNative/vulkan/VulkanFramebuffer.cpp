#include "stdafx.h"

VulkanFramebuffer::VulkanFramebuffer(VkDevice device, uint32_t width, uint32_t height, VulkanRenderPass renderPass, std::vector<VkImageView> &attachments)
{
    VkFramebufferCreateInfo framebufferInfo = {};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = renderPass.handle;
    framebufferInfo.attachmentCount = attachments.size();
    framebufferInfo.pAttachments = attachments.data();
    framebufferInfo.width = width;
    framebufferInfo.height = height;
    framebufferInfo.layers = 1;

    vkCreateFramebuffer(device, &framebufferInfo, nullptr, &handle);
}

VulkanFramebuffer::~VulkanFramebuffer()
{
}