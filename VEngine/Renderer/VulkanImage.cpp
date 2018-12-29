#include "stdafx.h" 
#include "VulkanImage.h" 
#include "VulkanImageFactory.h" 
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

        VulkanImage::VulkanImage(VulkanDevice * device, uint32_t width, uint32_t height, uint32_t depth, bool mipmapped,
            VEngineImageFormat format, VEngineImageUsage usage, VEngineImageAspect aspect, VEngineImageLayout layout)
            : format(format)
        {
            int mappedUsage = 0;// VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT
            if ((usage & VEngineImageUsage::ColorAttachment) == VEngineImageUsage::ColorAttachment)
                mappedUsage = mappedUsage | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
            if ((usage & VEngineImageUsage::Sampled) == VEngineImageUsage::Sampled)
                mappedUsage = mappedUsage | VK_IMAGE_USAGE_SAMPLED_BIT;
            if ((usage & VEngineImageUsage::Storage) == VEngineImageUsage::Storage)
                mappedUsage = mappedUsage | VK_IMAGE_USAGE_STORAGE_BIT;
            if ((usage & VEngineImageUsage::Depth) == VEngineImageUsage::Depth)
                mappedUsage = mappedUsage | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
            if ((usage & VEngineImageUsage::TransferDestination) == VEngineImageUsage::TransferDestination)
                mappedUsage = mappedUsage | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
            internalImage = new VulkanInternalImage(device, width, height, depth, resolveFormat(format), VK_IMAGE_TILING_OPTIMAL, (VkImageUsageFlagBits)mappedUsage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                aspect == VEngineImageAspect::ColorAspect ? VK_IMAGE_ASPECT_COLOR_BIT : VK_IMAGE_ASPECT_DEPTH_BIT,
                layout == VEngineImageLayout::Preinitialized ? VK_IMAGE_LAYOUT_PREINITIALIZED : VK_IMAGE_LAYOUT_UNDEFINED, mipmapped);
        }

        VulkanImage::VulkanImage(VulkanDevice * device, VulkanInternalImage * internalImage, VkFormat internalFormat)
            : device(device), internalImage(internalImage), format(reverseResolveFormat(internalFormat))
        {
        }

        VulkanImage::VulkanImage(VulkanDevice * device, FileSystem::MediaInterface* media, std::string mediakey)
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

        VkFormat VulkanImage::resolveFormat(VEngineImageFormat format)
        {
            switch (format) {
            case VEngineImageFormat::R8inorm: return VK_FORMAT_R8_SNORM;
            case VEngineImageFormat::RG8inorm: return VK_FORMAT_R8G8_SNORM;
            case VEngineImageFormat::RGBA8inorm: return VK_FORMAT_R8G8B8A8_SNORM;

            case VEngineImageFormat::R8unorm: return VK_FORMAT_R8_UNORM;
            case VEngineImageFormat::RG8unorm: return VK_FORMAT_R8G8_UNORM;
            case VEngineImageFormat::RGBA8unorm: return VK_FORMAT_R8G8B8A8_UNORM;

            case VEngineImageFormat::R16i: return VK_FORMAT_R16_SINT;
            case VEngineImageFormat::RG16i: return VK_FORMAT_R16G16_SINT;
            case VEngineImageFormat::RGBA16i: return VK_FORMAT_R16G16B16A16_SINT;

            case VEngineImageFormat::R16u: return VK_FORMAT_R16_UINT;
            case VEngineImageFormat::RG16u: return VK_FORMAT_R16G16_UINT;
            case VEngineImageFormat::RGBA16u: return VK_FORMAT_R16G16B16A16_UINT;

            case VEngineImageFormat::R16f: return VK_FORMAT_R16_SFLOAT;
            case VEngineImageFormat::RG16f: return VK_FORMAT_R16G16_SFLOAT;
            case VEngineImageFormat::RGBA16f: return VK_FORMAT_R16G16B16A16_SFLOAT;

            case VEngineImageFormat::R32i: return VK_FORMAT_R32_SINT;
            case VEngineImageFormat::RG32i: return VK_FORMAT_R32G32_SINT;
            case VEngineImageFormat::RGBA32i: return VK_FORMAT_R32G32B32A32_SINT;

            case VEngineImageFormat::R32u: return VK_FORMAT_R32_UINT;
            case VEngineImageFormat::RG32u: return VK_FORMAT_R32G32_UINT;
            case VEngineImageFormat::RGBA32u: return VK_FORMAT_R32G32B32A32_UINT;

            case VEngineImageFormat::R32f: return VK_FORMAT_R32_SFLOAT;
            case VEngineImageFormat::RG32f: return VK_FORMAT_R32G32_SFLOAT;
            case VEngineImageFormat::RGBA32f: return VK_FORMAT_R32G32B32A32_SFLOAT;

            case VEngineImageFormat::Depth16u: return VK_FORMAT_D16_UNORM;
            case VEngineImageFormat::Depth32f: return VK_FORMAT_D32_SFLOAT;

            case VEngineImageFormat::BGRA8unorm: return VK_FORMAT_B8G8R8A8_UNORM;
            case VEngineImageFormat::RGBA8srgb: return VK_FORMAT_R8G8B8A8_SRGB;
            case VEngineImageFormat::RGB5unormPack16: return VK_FORMAT_R5G6B5_UNORM_PACK16;
            case VEngineImageFormat::RGBA8snorm: return VK_FORMAT_R8G8B8A8_SNORM;
            case VEngineImageFormat::ABGR8unormPack32: return VK_FORMAT_A8B8G8R8_UNORM_PACK32;
            case VEngineImageFormat::ABGR8snormPack32: return VK_FORMAT_A8B8G8R8_SNORM_PACK32;
            case VEngineImageFormat::ABGR8srgbPack32: return VK_FORMAT_A8B8G8R8_SRGB_PACK32;
            case VEngineImageFormat::ARGB10unormPack32: return VK_FORMAT_A2R10G10B10_UNORM_PACK32;
            case VEngineImageFormat::ABGR10unormPack32: return VK_FORMAT_A2B10G10R10_UNORM_PACK32;
            case VEngineImageFormat::RGBA16unorm: return VK_FORMAT_R16G16B16A16_UNORM;
            case VEngineImageFormat::RGBA16snorm: return VK_FORMAT_R16G16B16A16_SNORM;
            case VEngineImageFormat::BGR11ufloatPack32: return VK_FORMAT_B10G11R11_UFLOAT_PACK32;
            case VEngineImageFormat::BGR5unormPack16: return VK_FORMAT_B5G6R5_UNORM_PACK16;
            case VEngineImageFormat::BGRA8snorm: return VK_FORMAT_B8G8R8A8_SNORM;
            case VEngineImageFormat::A1RGB5unormPack16: return VK_FORMAT_A1R5G5B5_UNORM_PACK16;
            case VEngineImageFormat::RGBA4unormPack16: return VK_FORMAT_R4G4B4A4_UNORM_PACK16;
            case VEngineImageFormat::BGRA4unormPack16: return VK_FORMAT_B4G4R4A4_UNORM_PACK16;
            case VEngineImageFormat::RGB4A1unormPack16: return VK_FORMAT_R5G5B5A1_UNORM_PACK16;
            case VEngineImageFormat::BGR5A1unormPack16: return VK_FORMAT_B5G5R5A1_UNORM_PACK16;
            }
            return VK_FORMAT_R8_SNORM;
        }

        VEngineImageFormat VulkanImage::reverseResolveFormat(VkFormat format)
        {
            switch (format) {
            case VK_FORMAT_R8_SNORM: return VEngineImageFormat::R8inorm;
            case VK_FORMAT_R8G8_SNORM: return VEngineImageFormat::RG8inorm;
            case VK_FORMAT_R8G8B8A8_SNORM: return VEngineImageFormat::RGBA8inorm;

            case VK_FORMAT_R8_UNORM: return VEngineImageFormat::R8unorm;
            case VK_FORMAT_R8G8_UNORM: return VEngineImageFormat::RG8unorm;
            case VK_FORMAT_R8G8B8A8_UNORM: return VEngineImageFormat::RGBA8unorm;

            case VK_FORMAT_R16_SINT: return VEngineImageFormat::R16i;
            case VK_FORMAT_R16G16_SINT: return VEngineImageFormat::RG16i;
            case VK_FORMAT_R16G16B16A16_SINT: return VEngineImageFormat::RGBA16i;

            case VK_FORMAT_R16_UINT: return VEngineImageFormat::R16u;
            case VK_FORMAT_R16G16_UINT: return VEngineImageFormat::RG16u;
            case VK_FORMAT_R16G16B16A16_UINT: return VEngineImageFormat::RGBA16u;

            case VK_FORMAT_R16_SFLOAT: return VEngineImageFormat::R16f;
            case VK_FORMAT_R16G16_SFLOAT: return VEngineImageFormat::RG16f;
            case VK_FORMAT_R16G16B16A16_SFLOAT: return VEngineImageFormat::RGBA16f;

            case VK_FORMAT_R32_SINT: return VEngineImageFormat::R32i;
            case VK_FORMAT_R32G32_SINT: return VEngineImageFormat::RG32i;
            case VK_FORMAT_R32G32B32A32_SINT: return VEngineImageFormat::RGBA32i;

            case VK_FORMAT_R32_UINT: return VEngineImageFormat::R32u;
            case VK_FORMAT_R32G32_UINT: return VEngineImageFormat::RG32u;
            case VK_FORMAT_R32G32B32A32_UINT: return VEngineImageFormat::RGBA32u;

            case VK_FORMAT_R32_SFLOAT: return VEngineImageFormat::R32f;
            case VK_FORMAT_R32G32_SFLOAT: return VEngineImageFormat::RG32f;
            case VK_FORMAT_R32G32B32A32_SFLOAT: return VEngineImageFormat::RGBA32f;

            case VK_FORMAT_D16_UNORM: return VEngineImageFormat::Depth16u;
            case VK_FORMAT_D32_SFLOAT: return VEngineImageFormat::Depth32f;

            case VK_FORMAT_R8G8B8A8_SRGB: return VEngineImageFormat::RGBA8srgb;
            case VK_FORMAT_R5G6B5_UNORM_PACK16: return VEngineImageFormat::RGB5unormPack16;
            case VK_FORMAT_A8B8G8R8_UNORM_PACK32: return VEngineImageFormat::ABGR8unormPack32;
            case VK_FORMAT_A8B8G8R8_SNORM_PACK32: return VEngineImageFormat::ABGR8snormPack32;
            case VK_FORMAT_A8B8G8R8_SRGB_PACK32: return VEngineImageFormat::ABGR8srgbPack32;
            case VK_FORMAT_A2R10G10B10_UNORM_PACK32: return VEngineImageFormat::ARGB10unormPack32;
            case VK_FORMAT_A2B10G10R10_UNORM_PACK32: return VEngineImageFormat::ABGR10unormPack32;
            case VK_FORMAT_R16G16B16A16_UNORM: return VEngineImageFormat::RGBA16unorm;
            case VK_FORMAT_R16G16B16A16_SNORM: return VEngineImageFormat::RGBA16snorm;
            case VK_FORMAT_B10G11R11_UFLOAT_PACK32: return VEngineImageFormat::BGR11ufloatPack32;
            case VK_FORMAT_B5G6R5_UNORM_PACK16: return VEngineImageFormat::BGR5unormPack16;
            case VK_FORMAT_B8G8R8A8_SNORM: return VEngineImageFormat::BGRA8snorm;
            case VK_FORMAT_B8G8R8A8_UNORM: return VEngineImageFormat::BGRA8unorm;
            case VK_FORMAT_A1R5G5B5_UNORM_PACK16: return VEngineImageFormat::A1RGB5unormPack16;
            case VK_FORMAT_R4G4B4A4_UNORM_PACK16: return VEngineImageFormat::RGBA4unormPack16;
            case VK_FORMAT_B4G4R4A4_UNORM_PACK16: return VEngineImageFormat::BGRA4unormPack16;
            case VK_FORMAT_R5G5B5A1_UNORM_PACK16: return VEngineImageFormat::RGB4A1unormPack16;
            case VK_FORMAT_B5G5R5A1_UNORM_PACK16: return VEngineImageFormat::BGR5A1unormPack16;
            }
            return VEngineImageFormat::R8inorm;
        }

        void VulkanImage::regenerateMipmaps()
        {
            internalImage->regenerateMipmaps();
        }

        AttachmentInterface* VulkanImage::getAttachment(VEngineAttachmentBlending blending, bool clear, VEngineClearColorValue clearColor, bool forPresent)
        {
            auto att = new VulkanAttachment(this, 
                resolveFormat(format), VK_SAMPLE_COUNT_1_BIT,
                clear ? VK_ATTACHMENT_LOAD_OP_CLEAR : (VK_ATTACHMENT_LOAD_OP_LOAD), VK_ATTACHMENT_STORE_OP_STORE,
                VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE,
                VK_IMAGE_LAYOUT_UNDEFINED, forPresent ? VK_IMAGE_LAYOUT_PRESENT_SRC_KHR : (isDepthBuffer() ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_GENERAL),
                blending, clearColor, clear);
            return att;
        }

        AttachmentInterface * VulkanImage::getAttachment(VEngineAttachmentBlending blending, bool clear, VEngineClearColorValue clearColor)
        {
            return getAttachment(blending, clear, clearColor, false);
        }

        AttachmentInterface * VulkanImage::getAttachment(VEngineAttachmentBlending blending, bool clear)
        {
            return getAttachment(blending, clear, { {} }, false);
        }

        AttachmentInterface * VulkanImage::getAttachment(VEngineAttachmentBlending blending, VEngineClearColorValue clearColor)
        {
            return getAttachment(blending, true, clearColor, false);
        }

        AttachmentInterface * VulkanImage::getAttachment(VEngineAttachmentBlending blending)
        {
            return getAttachment(blending, true, { {0.0f, 0.0f, 0.0f, 0.0f} }, false);
        }

        bool VulkanImage::isDepthBuffer()
        {
            return VulkanImageFactory::internalResolveIsDepthBuffer(format);
        }

        VkSampler VulkanImage::getSampler()
        {
            return internalImage->getSampler();
        }

        VkImageView VulkanImage::getImageView()
        {
            return internalImage->getImageView();
        }

        VkImageView VulkanImage::getFirstMipmapImageView()
        {
            return internalImage->getFirstMipmapImageView();
        }

    }
}