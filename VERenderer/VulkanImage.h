#pragma once 
class VulkanDevice;
#include "Internal/VulkanSingleAllocation.h"
class VulkanInternalImage;
class VulkanAttachment;
#include "Internal/VulkanAttachment.h" 

enum VulkanImageUsage {
    ColorAttachment = 1, 
    Sampled = 2, 
    Storage = 4, 
    Depth = 8, 
    TransferDestination = 16
};

enum VulkanImageAspect {
    ColorAspect,
    DepthAspect
};

enum VulkanImageLayout {
    Preinitialized,
    Undefined
};

enum VulkanImageFormat {
    R8inorm,
    RG8inorm,
    RGBA8inorm,

    R8unorm,
    RG8unorm,
    RGBA8unorm,

    R16i,
    RG16i,
    RGBA16i,

    R16u,
    RG16u,
    RGBA16u,

    R16f,
    RG16f,
    RGBA16f,

    R32i,
    RG32i,
    RGBA32i,

    R32u,
    RG32u,
    RGBA32u,

    R32f,
    RG32f,
    RGBA32f,

    Depth16u,
    Depth32f
};

class VulkanImage
{
public:
    VulkanImage(VulkanDevice* device, uint32_t width, uint32_t height, uint32_t depth, 
        VulkanImageFormat format, VulkanImageUsage usage, VulkanImageAspect aspect, VulkanImageLayout layout);
    VulkanImage(VulkanDevice* device, VulkanInternalImage* internalImage, VkFormat internalFormat);
    ~VulkanImage();
    // todo abstarct out VkClearColorValue
    VulkanAttachment* getAttachment(VulkanAttachmentBlending blending, bool clear, VkClearColorValue clearColor, bool forPresent);
    bool isDepthBuffer();
    VkSampler getSampler();
    VkImageView getImageView();
private:
    VkFormat resolveFormat(VulkanImageFormat format);
    VulkanImageFormat reverseResolveFormat(VkFormat format);
    bool resolveIsDepthBuffer(VulkanImageFormat format);
    VulkanDevice * device;
    VulkanInternalImage * internalImage;
    VulkanImageFormat format;
};
