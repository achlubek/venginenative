#pragma once
class VulkanAttachment
{
public:
	VulkanAttachment() {}
    VulkanAttachment(VkFormat format, VkSampleCountFlagBits samples, VkAttachmentLoadOp loadop, VkAttachmentStoreOp storeop,
        VkAttachmentLoadOp stencilloadop, VkAttachmentStoreOp stencilstoreop, VkImageLayout initialLayout, VkImageLayout finalLayout);
    ~VulkanAttachment();
    VkAttachmentDescription description;
    VkAttachmentReference getReference(uint32_t attachment, VkImageLayout layout);
};

