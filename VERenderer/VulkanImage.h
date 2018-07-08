#pragma once 
class VulkanToolkit;
#include "Internal/VulkanSingleAllocation.h"
class VulkanInternalImage;
class VulkanAttachment;

enum VulkanImageUsage {
    ColorAttachment = 1, 
    Sampled = 2, 
    Storage = 4, 
    Depth = 8, 
    TransferDestination = 16
};

enum VulkanImageAspect {
    Color,
    Depth
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
    VulkanImage(VulkanDevice* device, uint32_t width, uint32_t height, uint32_t depth, VulkanImageFormat format, VulkanImageUsage usage, VulkanImageAspect aspect, VulkanImageLayout layout);
    ~VulkanImage();
    VulkanAttachment getAttachment();
private:
    VkFormat resolveFormat(VulkanImageFormat format);
    VulkanDevice * device;
    VulkanInternalImage * device;
};
