#include "stdafx.h"


VulkanAttachment::VulkanAttachment(VulkanImage* iimage, VkFormat format, VkSampleCountFlagBits samples, VkAttachmentLoadOp loadop, VkAttachmentStoreOp storeop, 
    VkAttachmentLoadOp stencilloadop, VkAttachmentStoreOp stencilstoreop, VkImageLayout initialLayout, VkImageLayout finalLayout)
{
    image = iimage;
    description = {};
    description.format = format;
    description.samples = samples;
    description.loadOp = loadop;
    description.storeOp = storeop;
    description.stencilLoadOp = stencilloadop;
    description.stencilStoreOp = stencilstoreop;
    description.initialLayout = initialLayout;
    description.finalLayout = finalLayout;
    blending = VulkanAttachmentBlending::None;
    clearColor = { 0.0f, 0.0f, 0.0f, 0.0f };

}


VulkanAttachment::~VulkanAttachment()
{
}

VkAttachmentReference VulkanAttachment::getReference(uint32_t attachment, VkImageLayout layout)
{
    VkAttachmentReference ref = {};
    ref.attachment = attachment;
    ref.layout = layout;
    return ref;
}
