#include "stdafx.h"
#include "VulkanAttachment.h"

VulkanAttachment::VulkanAttachment(VulkanImage* iimage, VkFormat format, VkSampleCountFlagBits samples, VkAttachmentLoadOp loadop, VkAttachmentStoreOp storeop, 
    VkAttachmentLoadOp stencilloadop, VkAttachmentStoreOp stencilstoreop, VkImageLayout initialLayout, VkImageLayout finalLayout,
    VulkanAttachmentBlending blending, VkClearColorValue clearColor, bool clear)
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
    blending = blending;
    clearColor = clearColor;
    clear = clear;
}


VulkanAttachment::~VulkanAttachment()
{
}

VulkanImage * VulkanAttachment::getImage()
{
    return image;
}

VkAttachmentDescription VulkanAttachment::getDescription()
{
    return description;
}

VulkanAttachmentBlending VulkanAttachment::getBlending()
{
    return blending;
}

VkClearColorValue VulkanAttachment::getClearColor()
{
    return clearColor;
}

bool VulkanAttachment::isCleared()
{
    return clear;
}

VkAttachmentReference VulkanAttachment::getReference(uint32_t attachment, VkImageLayout layout)
{
    VkAttachmentReference ref = {};
    ref.attachment = attachment;
    ref.layout = layout;
    return ref;
}
