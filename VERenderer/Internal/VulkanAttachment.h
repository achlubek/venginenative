#pragma once
class VulkanImage;
enum VulkanAttachmentBlending {
    None,
    Additive,
    Alpha
};
class VulkanAttachment
{
public:
    VulkanAttachment() {}
    VulkanImage* image;
    VulkanAttachment(VulkanImage* iimage, VkFormat format, VkSampleCountFlagBits samples, VkAttachmentLoadOp loadop, VkAttachmentStoreOp storeop,
        VkAttachmentLoadOp stencilloadop, VkAttachmentStoreOp stencilstoreop, VkImageLayout initialLayout, VkImageLayout finalLayout);
    ~VulkanAttachment();
    VkAttachmentDescription description;
    VkAttachmentReference getReference(uint32_t attachment, VkImageLayout layout);
    VulkanAttachmentBlending blending;
    VkClearColorValue clearColor;
    bool clear = true;
};

