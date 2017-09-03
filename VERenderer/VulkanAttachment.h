#pragma once
class VulkanImage;
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
};

