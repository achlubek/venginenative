#pragma once
class VulkanImage;
enum VulkanAttachmentBlending {
    None,
    Additive,
    Alpha
};

class VulkanAttachmentClearColor {
    float       float32[4];
    int32_t     int32[4];
    uint32_t    uint32[4];
};


class VulkanAttachment
{
public:
    VulkanAttachment() {} // todo kurwa co to jest
    VulkanAttachment(VulkanImage* iimage, VkFormat format, VkSampleCountFlagBits samples, VkAttachmentLoadOp loadop, VkAttachmentStoreOp storeop,
        VkAttachmentLoadOp stencilloadop, VkAttachmentStoreOp stencilstoreop, VkImageLayout initialLayout, VkImageLayout finalLayout, 
        VulkanAttachmentBlending blending, VkClearColorValue clearColor, bool clear);
    ~VulkanAttachment();
    VulkanImage* getImage();
    VkAttachmentDescription getDescription();
    VulkanAttachmentBlending getBlending();
    VkClearColorValue getClearColor();
    VkAttachmentReference getReference(uint32_t attachment, VkImageLayout layout);
    bool isCleared();
private:
    VulkanImage* image;
    VkAttachmentDescription description;
    VulkanAttachmentBlending blending;
    VkClearColorValue clearColor;
    bool clear = true;
};

