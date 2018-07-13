#include "stdafx.h" 
#include "VulkanImage.h" 
#include "Internal/VulkanInternalImage.h"
#include "Internal/VulkanMemoryManager.h" 
#include "Internal/VulkanMemoryChunk.h" 
#include "Internal/VulkanAttachment.h" 


VulkanImage::VulkanImage(VulkanDevice * device, uint32_t width, uint32_t height, uint32_t depth, 
    VulkanImageFormat format, VulkanImageUsage usage, VulkanImageAspect aspect, VulkanImageLayout layout)
    : format(format)
{
    int mappedUsage = 0;// VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT
    if ((usage & VulkanImageUsage::ColorAttachment) == VulkanImageUsage::ColorAttachment) 
        mappedUsage = mappedUsage | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    if ((usage & VulkanImageUsage::Sampled) == VulkanImageUsage::Sampled) 
        mappedUsage = mappedUsage | VK_IMAGE_USAGE_SAMPLED_BIT;
    if ((usage & VulkanImageUsage::Storage) == VulkanImageUsage::Storage) 
        mappedUsage = mappedUsage | VK_IMAGE_USAGE_STORAGE_BIT;
    if ((usage & VulkanImageUsage::Depth) == VulkanImageUsage::Depth) 
        mappedUsage = mappedUsage | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    if ((usage & VulkanImageUsage::TransferDestination) == VulkanImageUsage::TransferDestination) 
        mappedUsage = mappedUsage | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    internalImage = new VulkanInternalImage(device, width, height, depth, resolveFormat(format), VK_IMAGE_TILING_OPTIMAL, (VkImageUsageFlagBits)mappedUsage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        aspect == VulkanImageAspect::ColorAspect ? VK_IMAGE_ASPECT_COLOR_BIT : VK_IMAGE_ASPECT_DEPTH_BIT,
        layout == VulkanImageLayout::Preinitialized ? VK_IMAGE_LAYOUT_PREINITIALIZED : VK_IMAGE_LAYOUT_UNDEFINED);
}

VulkanImage::VulkanImage(VulkanDevice * device, VulkanInternalImage * internalImage, VkFormat internalFormat)
    : device(device), internalImage(internalImage), format(reverseResolveFormat(internalFormat))
{
}

VulkanImage::~VulkanImage()
{
}

VkFormat VulkanImage::resolveFormat(VulkanImageFormat format)
{
    switch (format) {
    case VulkanImageFormat::R8inorm: return VK_FORMAT_R8_SNORM;
    case VulkanImageFormat::RG8inorm: return VK_FORMAT_R8G8_SNORM;
    case VulkanImageFormat::RGBA8inorm: return VK_FORMAT_R8G8B8A8_SNORM;

    case VulkanImageFormat::R8unorm: return VK_FORMAT_R8_UNORM;
    case VulkanImageFormat::RG8unorm: return VK_FORMAT_R8G8_UNORM;
    case VulkanImageFormat::RGBA8unorm: return VK_FORMAT_R8G8B8A8_UNORM;

    case VulkanImageFormat::R16i: return VK_FORMAT_R16_SINT;
    case VulkanImageFormat::RG16i: return VK_FORMAT_R16G16_SINT;
    case VulkanImageFormat::RGBA16i: return VK_FORMAT_R16G16B16A16_SINT;

    case VulkanImageFormat::R16u: return VK_FORMAT_R16_UINT;
    case VulkanImageFormat::RG16u: return VK_FORMAT_R16G16_UINT;
    case VulkanImageFormat::RGBA16u: return VK_FORMAT_R16G16B16A16_UINT;

    case VulkanImageFormat::R16f: return VK_FORMAT_R16_SFLOAT;
    case VulkanImageFormat::RG16f: return VK_FORMAT_R16G16_SFLOAT;
    case VulkanImageFormat::RGBA16f: return VK_FORMAT_R16G16B16A16_SFLOAT;

    case VulkanImageFormat::R32i: return VK_FORMAT_R32_SINT;
    case VulkanImageFormat::RG32i: return VK_FORMAT_R32G32_SINT;
    case VulkanImageFormat::RGBA32i: return VK_FORMAT_R32G32B32A32_SINT;

    case VulkanImageFormat::R32u: return VK_FORMAT_R32_UINT;
    case VulkanImageFormat::RG32u: return VK_FORMAT_R32G32_UINT;
    case VulkanImageFormat::RGBA32u: return VK_FORMAT_R32G32B32A32_UINT;

    case VulkanImageFormat::R32f: return VK_FORMAT_R32_SFLOAT;
    case VulkanImageFormat::RG32f: return VK_FORMAT_R32G32_SFLOAT;
    case VulkanImageFormat::RGBA32f: return VK_FORMAT_R32G32B32A32_SFLOAT;

    case VulkanImageFormat::Depth16u: return VK_FORMAT_D16_UNORM;
    case VulkanImageFormat::Depth32f: return VK_FORMAT_D32_SFLOAT;
    }
    return VK_FORMAT_R8_SNORM;
}

VulkanImageFormat VulkanImage::reverseResolveFormat(VkFormat format)
{
    switch (format) {
    case VK_FORMAT_R8_SNORM: return VulkanImageFormat::R8inorm;
    case VK_FORMAT_R8G8_SNORM: return VulkanImageFormat::RG8inorm;
    case VK_FORMAT_R8G8B8A8_SNORM: return VulkanImageFormat::RGBA8inorm;

    case VK_FORMAT_R8_UNORM: return VulkanImageFormat::R8unorm;
    case VK_FORMAT_R8G8_UNORM: return VulkanImageFormat::RG8unorm;
    case VK_FORMAT_R8G8B8A8_UNORM: return VulkanImageFormat::RGBA8unorm;

    case VK_FORMAT_R16_SINT: return VulkanImageFormat::R16i;
    case VK_FORMAT_R16G16_SINT: return VulkanImageFormat::RG16i;
    case VK_FORMAT_R16G16B16A16_SINT: return VulkanImageFormat::RGBA16i;

    case VK_FORMAT_R16_UINT: return VulkanImageFormat::R16u;
    case VK_FORMAT_R16G16_UINT: return VulkanImageFormat::RG16u;
    case VK_FORMAT_R16G16B16A16_UINT: return VulkanImageFormat::RGBA16u;

    case VK_FORMAT_R16_SFLOAT: return VulkanImageFormat::R16f;
    case VK_FORMAT_R16G16_SFLOAT: return VulkanImageFormat::RG16f;
    case VK_FORMAT_R16G16B16A16_SFLOAT: return VulkanImageFormat::RGBA16f;

    case VK_FORMAT_R32_SINT: return VulkanImageFormat::R32i;
    case VK_FORMAT_R32G32_SINT: return VulkanImageFormat::RG32i;
    case VK_FORMAT_R32G32B32A32_SINT: return VulkanImageFormat::RGBA32i;

    case VK_FORMAT_R32_UINT: return VulkanImageFormat::R32u;
    case VK_FORMAT_R32G32_UINT: return VulkanImageFormat::RG32u;
    case VK_FORMAT_R32G32B32A32_UINT: return VulkanImageFormat::RGBA32u;

    case VK_FORMAT_R32_SFLOAT: return VulkanImageFormat::R32f;
    case VK_FORMAT_R32G32_SFLOAT: return VulkanImageFormat::RG32f;
    case VK_FORMAT_R32G32B32A32_SFLOAT: return VulkanImageFormat::RGBA32f;

    case VK_FORMAT_D16_UNORM: return VulkanImageFormat::Depth16u;
    case VK_FORMAT_D32_SFLOAT: return VulkanImageFormat::Depth32f;
    }
    return VulkanImageFormat::R8inorm;
}

bool VulkanImage::resolveIsDepthBuffer(VulkanImageFormat format)
{
    switch (format) {
    case VulkanImageFormat::R8inorm: 
    case VulkanImageFormat::RG8inorm: 
    case VulkanImageFormat::RGBA8inorm: 

    case VulkanImageFormat::R8unorm:
    case VulkanImageFormat::RG8unorm: 
    case VulkanImageFormat::RGBA8unorm:

    case VulkanImageFormat::R16i: 
    case VulkanImageFormat::RG16i: 
    case VulkanImageFormat::RGBA16i: 

    case VulkanImageFormat::R16u: 
    case VulkanImageFormat::RG16u: 
    case VulkanImageFormat::RGBA16u:

    case VulkanImageFormat::R16f: 
    case VulkanImageFormat::RG16f: 
    case VulkanImageFormat::RGBA16f: 

    case VulkanImageFormat::R32i:
    case VulkanImageFormat::RG32i: 
    case VulkanImageFormat::RGBA32i:

    case VulkanImageFormat::R32u: 
    case VulkanImageFormat::RG32u: 
    case VulkanImageFormat::RGBA32u:

    case VulkanImageFormat::R32f: 
    case VulkanImageFormat::RG32f: 
    case VulkanImageFormat::RGBA32f: return false;

    case VulkanImageFormat::Depth16u:;
    case VulkanImageFormat::Depth32f: return true;
    }
    return false;
}

VulkanAttachment* VulkanImage::getAttachment(VulkanAttachmentBlending blending, bool clear, VkClearColorValue clearColor, bool forPresent)
{
    auto att = new VulkanAttachment(this, resolveFormat(format), VK_SAMPLE_COUNT_1_BIT,
        clear ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD, VK_ATTACHMENT_STORE_OP_STORE,
        VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE,
        VK_IMAGE_LAYOUT_UNDEFINED, forPresent ? VK_IMAGE_LAYOUT_PRESENT_SRC_KHR  : (resolveIsDepthBuffer(format) ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL),
        blending, clearColor, clear);
    return att;
}

bool VulkanImage::isDepthBuffer()
{
    return resolveIsDepthBuffer(format);
}

VkSampler VulkanImage::getSampler()
{
    return internalImage->getSampler();
}

VkImageView VulkanImage::getImageView()
{
    return internalImage->getImageView();
}
