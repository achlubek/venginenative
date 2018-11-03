#include "stdafx.h" 
#include "VulkanImage.h" 
#include "Internal/VulkanInternalImage.h"
#include "Internal/VulkanMemoryManager.h" 
#include "Internal/VulkanMemoryChunk.h" 
#include "VulkanAttachment.h" 

namespace VEngine
{
    namespace Renderer
    {
        using namespace VEngine::Renderer::Internal;
        using namespace VEngine::FileSystem;

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

        VulkanImage::VulkanImage(VulkanDevice * device, FileSystem::Media* media, std::string mediakey)
            : device(device), internalImage(new VulkanInternalImage(device, media, mediakey))
        {
            format = reverseResolveFormat(internalImage->getFormat());
        }

        VulkanImage::VulkanImage(VulkanDevice * device, uint32_t width, uint32_t height, uint32_t channelCount, void * data)
            : device(device), internalImage(new VulkanInternalImage(device, width, height, channelCount, data))
        {
            format = reverseResolveFormat(internalImage->getFormat());
        }

        VulkanImage::~VulkanImage()
        {
            safedelete(internalImage);
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

            case VulkanImageFormat::BGRA8unorm: return VK_FORMAT_B8G8R8A8_UNORM;
            case VulkanImageFormat::RGBA8srgb: return VK_FORMAT_R8G8B8A8_SRGB;
            case VulkanImageFormat::RGB5unormPack16: return VK_FORMAT_R5G6B5_UNORM_PACK16;
            case VulkanImageFormat::RGBA8snorm: return VK_FORMAT_R8G8B8A8_SNORM;
            case VulkanImageFormat::ABGR8unormPack32: return VK_FORMAT_A8B8G8R8_UNORM_PACK32;
            case VulkanImageFormat::ABGR8snormPack32: return VK_FORMAT_A8B8G8R8_SNORM_PACK32;
            case VulkanImageFormat::ABGR8srgbPack32: return VK_FORMAT_A8B8G8R8_SRGB_PACK32;
            case VulkanImageFormat::ARGB10unormPack32: return VK_FORMAT_A2R10G10B10_UNORM_PACK32;
            case VulkanImageFormat::ABGR10unormPack32: return VK_FORMAT_A2B10G10R10_UNORM_PACK32;
            case VulkanImageFormat::RGBA16unorm: return VK_FORMAT_R16G16B16A16_UNORM;
            case VulkanImageFormat::RGBA16snorm: return VK_FORMAT_R16G16B16A16_SNORM;
            case VulkanImageFormat::BGR11ufloatPack32: return VK_FORMAT_B10G11R11_UFLOAT_PACK32;
            case VulkanImageFormat::BGR5unormPack16: return VK_FORMAT_B5G6R5_UNORM_PACK16;
            case VulkanImageFormat::BGRA8snorm: return VK_FORMAT_B8G8R8A8_SNORM;
            case VulkanImageFormat::A1RGB5unormPack16: return VK_FORMAT_A1R5G5B5_UNORM_PACK16;
            case VulkanImageFormat::RGBA4unormPack16: return VK_FORMAT_R4G4B4A4_UNORM_PACK16;
            case VulkanImageFormat::BGRA4unormPack16: return VK_FORMAT_B4G4R4A4_UNORM_PACK16;
            case VulkanImageFormat::RGB4A1unormPack16: return VK_FORMAT_R5G5B5A1_UNORM_PACK16;
            case VulkanImageFormat::BGR5A1unormPack16: return VK_FORMAT_B5G5R5A1_UNORM_PACK16;
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

            case VK_FORMAT_R8G8B8A8_SRGB: return VulkanImageFormat::RGBA8srgb;
            case VK_FORMAT_R5G6B5_UNORM_PACK16: return VulkanImageFormat::RGB5unormPack16;
            case VK_FORMAT_A8B8G8R8_UNORM_PACK32: return VulkanImageFormat::ABGR8unormPack32;
            case VK_FORMAT_A8B8G8R8_SNORM_PACK32: return VulkanImageFormat::ABGR8snormPack32;
            case VK_FORMAT_A8B8G8R8_SRGB_PACK32: return VulkanImageFormat::ABGR8srgbPack32;
            case VK_FORMAT_A2R10G10B10_UNORM_PACK32: return VulkanImageFormat::ARGB10unormPack32;
            case VK_FORMAT_A2B10G10R10_UNORM_PACK32: return VulkanImageFormat::ABGR10unormPack32;
            case VK_FORMAT_R16G16B16A16_UNORM: return VulkanImageFormat::RGBA16unorm;
            case VK_FORMAT_R16G16B16A16_SNORM: return VulkanImageFormat::RGBA16snorm;
            case VK_FORMAT_B10G11R11_UFLOAT_PACK32: return VulkanImageFormat::BGR11ufloatPack32;
            case VK_FORMAT_B5G6R5_UNORM_PACK16: return VulkanImageFormat::BGR5unormPack16;
            case VK_FORMAT_B8G8R8A8_SNORM: return VulkanImageFormat::BGRA8snorm;
            case VK_FORMAT_B8G8R8A8_UNORM: return VulkanImageFormat::BGRA8unorm;
            case VK_FORMAT_A1R5G5B5_UNORM_PACK16: return VulkanImageFormat::A1RGB5unormPack16;
            case VK_FORMAT_R4G4B4A4_UNORM_PACK16: return VulkanImageFormat::RGBA4unormPack16;
            case VK_FORMAT_B4G4R4A4_UNORM_PACK16: return VulkanImageFormat::BGRA4unormPack16;
            case VK_FORMAT_R5G5B5A1_UNORM_PACK16: return VulkanImageFormat::RGB4A1unormPack16;
            case VK_FORMAT_B5G5R5A1_UNORM_PACK16: return VulkanImageFormat::BGR5A1unormPack16;
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

            case VulkanImageFormat::Depth16u:
            case VulkanImageFormat::Depth32f: return true;
            }
            return false;
        }

        VulkanAttachment* VulkanImage::getAttachment(VulkanAttachmentBlending blending, bool clear, VkClearColorValue clearColor, bool forPresent)
        {
            auto att = new VulkanAttachment(this, resolveFormat(format), VK_SAMPLE_COUNT_1_BIT,
                clear ? VK_ATTACHMENT_LOAD_OP_CLEAR : (VK_ATTACHMENT_LOAD_OP_LOAD), VK_ATTACHMENT_STORE_OP_STORE,
                VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE,
                VK_IMAGE_LAYOUT_UNDEFINED, forPresent ? VK_IMAGE_LAYOUT_PRESENT_SRC_KHR : (resolveIsDepthBuffer(format) ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL),
                blending, clearColor, clear);
            return att;
        }

        VulkanAttachment * VulkanImage::getAttachment(VulkanAttachmentBlending blending, bool clear, VkClearColorValue clearColor)
        {
            return getAttachment(blending, clear, clearColor, false);
        }

        VulkanAttachment * VulkanImage::getAttachment(VulkanAttachmentBlending blending, bool clear)
        {
            return getAttachment(blending, clear, { {} }, false);
        }

        VulkanAttachment * VulkanImage::getAttachment(VulkanAttachmentBlending blending, VkClearColorValue clearColor)
        {
            return getAttachment(blending, true, clearColor, false);
        }

        VulkanAttachment * VulkanImage::getAttachment(VulkanAttachmentBlending blending)
        {
            return getAttachment(blending, true, { {0.0f, 0.0f, 0.0f, 0.0f} }, false);
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

    }
}