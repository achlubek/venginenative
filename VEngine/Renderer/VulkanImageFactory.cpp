#include "stdafx.h"
#include "VulkanImageFactory.h"
#include "VulkanImage.h"
#include "../Interface/FileSystem/MediaInterface.h"

namespace VEngine
{
    namespace Renderer
    {
        using namespace VEngine::Renderer::Internal;
        using namespace VEngine::FileSystem;

        VulkanImageFactory::VulkanImageFactory(VulkanDevice* device, FileSystem::MediaInterface * media)
            : device(device), media(media)
        {
        }

        VulkanImageFactory::~VulkanImageFactory()
        {
        }

        ImageInterface * VulkanImageFactory::build(uint32_t width, uint32_t height, uint32_t depth, VEngineImageFormat format, VEngineImageUsage usage, VEngineImageAspect aspect, VEngineImageLayout layout)
        {
            return new VulkanImage(device, width, height, depth, false, format, usage, aspect, layout);
        }

        ImageInterface * VulkanImageFactory::build(uint32_t width, uint32_t height, uint32_t depth, VEngineImageFormat format, VEngineImageUsage usage)
        {
            return build(width, height, depth, format, usage, resolveIsDepthBuffer(format) ? VEngineImageAspect::DepthAspect : VEngineImageAspect::ColorAspect, VEngineImageLayout::Preinitialized);
        }

        ImageInterface * VulkanImageFactory::build(uint32_t width, uint32_t height, VEngineImageFormat format, VEngineImageUsage usage, VEngineImageAspect aspect, VEngineImageLayout layout)
        {
            return build(width, height, 1, format, usage, aspect, layout);
        }

        ImageInterface * VulkanImageFactory::build(uint32_t width, uint32_t height, VEngineImageFormat format, VEngineImageUsage usage)
        {
            return build(width, height, 1, format, usage, resolveIsDepthBuffer(format) ? VEngineImageAspect::DepthAspect : VEngineImageAspect::ColorAspect, VEngineImageLayout::Preinitialized);
        }

        ImageInterface * VulkanImageFactory::build(uint32_t width, uint32_t height, VEngineImageFormat format, int usage)
        {
            return build(width, height, 1, format, static_cast<VEngineImageUsage>(usage), resolveIsDepthBuffer(format) ? VEngineImageAspect::DepthAspect : VEngineImageAspect::ColorAspect, VEngineImageLayout::Preinitialized);
        }

        ImageInterface * VulkanImageFactory::buildMipmapped(uint32_t width, uint32_t height, uint32_t depth, VEngineImageFormat format, VEngineImageUsage usage, VEngineImageAspect aspect, VEngineImageLayout layout)
        {
            return new VulkanImage(device, width, height, depth, true, format, usage, aspect, layout);
        }

        ImageInterface * VulkanImageFactory::buildMipmapped(uint32_t width, uint32_t height, uint32_t depth, VEngineImageFormat format, VEngineImageUsage usage)
        {
            return buildMipmapped(width, height, depth, format, usage, resolveIsDepthBuffer(format) ? VEngineImageAspect::DepthAspect : VEngineImageAspect::ColorAspect, VEngineImageLayout::Preinitialized);
        }

        ImageInterface * VulkanImageFactory::buildMipmapped(uint32_t width, uint32_t height, VEngineImageFormat format, VEngineImageUsage usage, VEngineImageAspect aspect, VEngineImageLayout layout)
        {
            return buildMipmapped(width, height, 1, format, usage, aspect, layout);
        }

        ImageInterface * VulkanImageFactory::buildMipmapped(uint32_t width, uint32_t height, VEngineImageFormat format, VEngineImageUsage usage)
        {
            return buildMipmapped(width, height, 1, format, usage, resolveIsDepthBuffer(format) ? VEngineImageAspect::DepthAspect : VEngineImageAspect::ColorAspect, VEngineImageLayout::Preinitialized);
        }

        ImageInterface * VulkanImageFactory::buildMipmapped(uint32_t width, uint32_t height, VEngineImageFormat format, int usage)
        {
            return buildMipmapped(width, height, 1, format, static_cast<VEngineImageUsage>(usage), resolveIsDepthBuffer(format) ? VEngineImageAspect::DepthAspect : VEngineImageAspect::ColorAspect, VEngineImageLayout::Preinitialized);
        }

        ImageInterface * VulkanImageFactory::build(std::string mediakey)
        {
            return new VulkanImage(device, media, mediakey);
        }

        ImageInterface * VulkanImageFactory::build(uint32_t width, uint32_t height, uint32_t channelCount, void * data)
        {
            return new VulkanImage(device, width, height, channelCount, data);
        }

        bool VulkanImageFactory::resolveIsDepthBuffer(VEngineImageFormat format)
        {
            return internalResolveIsDepthBuffer(format);
        }

        bool VulkanImageFactory::internalResolveIsDepthBuffer(VEngineImageFormat format)
        {
            switch (format) {
            case VEngineImageFormat::R8inorm:
            case VEngineImageFormat::RG8inorm:
            case VEngineImageFormat::RGBA8inorm:

            case VEngineImageFormat::R8unorm:
            case VEngineImageFormat::RG8unorm:
            case VEngineImageFormat::RGBA8unorm:

            case VEngineImageFormat::R16i:
            case VEngineImageFormat::RG16i:
            case VEngineImageFormat::RGBA16i:

            case VEngineImageFormat::R16u:
            case VEngineImageFormat::RG16u:
            case VEngineImageFormat::RGBA16u:

            case VEngineImageFormat::R16f:
            case VEngineImageFormat::RG16f:
            case VEngineImageFormat::RGBA16f:

            case VEngineImageFormat::R32i:
            case VEngineImageFormat::RG32i:
            case VEngineImageFormat::RGBA32i:

            case VEngineImageFormat::R32u:
            case VEngineImageFormat::RG32u:
            case VEngineImageFormat::RGBA32u:

            case VEngineImageFormat::R32f:
            case VEngineImageFormat::RG32f:
            case VEngineImageFormat::RGBA32f: return false;

            case VEngineImageFormat::Depth16u:
            case VEngineImageFormat::Depth32f: return true;
            }
            return false;
        }

    }
}