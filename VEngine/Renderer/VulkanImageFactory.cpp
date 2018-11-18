#include "stdafx.h"
#include "VulkanImageFactory.h"
#include "VulkanImage.h"

namespace VEngine
{
    namespace Renderer
    {
        using namespace VEngine::Renderer::Internal;
        using namespace VEngine::FileSystem;

        VulkanImageFactory::VulkanImageFactory(VulkanDevice* device, FileSystem::Media * media)
            : device(device), media(media)
        {
        }

        VulkanImageFactory::~VulkanImageFactory()
        {
        }

        VulkanImage * VulkanImageFactory::build(uint32_t width, uint32_t height, uint32_t depth, VulkanImageFormat format, VulkanImageUsage usage, VulkanImageAspect aspect, VulkanImageLayout layout)
        {
            return new VulkanImage(device, width, height, depth, false, format, usage, aspect, layout);
        }

        VulkanImage * VulkanImageFactory::build(uint32_t width, uint32_t height, uint32_t depth, VulkanImageFormat format, VulkanImageUsage usage)
        {
            return build(width, height, depth, format, usage, VulkanImage::resolveIsDepthBuffer(format) ? VulkanImageAspect::DepthAspect : VulkanImageAspect::ColorAspect, VulkanImageLayout::Preinitialized);
        }

        VulkanImage * VulkanImageFactory::build(uint32_t width, uint32_t height, VulkanImageFormat format, VulkanImageUsage usage, VulkanImageAspect aspect, VulkanImageLayout layout)
        {
            return build(width, height, 1, format, usage, aspect, layout);
        }

        VulkanImage * VulkanImageFactory::build(uint32_t width, uint32_t height, VulkanImageFormat format, VulkanImageUsage usage)
        {
            return build(width, height, 1, format, usage, VulkanImage::resolveIsDepthBuffer(format) ? VulkanImageAspect::DepthAspect : VulkanImageAspect::ColorAspect, VulkanImageLayout::Preinitialized);
        }

        VulkanImage * VulkanImageFactory::build(uint32_t width, uint32_t height, VulkanImageFormat format, int usage)
        {
            return build(width, height, 1, format, static_cast<VulkanImageUsage>(usage), VulkanImage::resolveIsDepthBuffer(format) ? VulkanImageAspect::DepthAspect : VulkanImageAspect::ColorAspect, VulkanImageLayout::Preinitialized);
        }

        VulkanImage * VulkanImageFactory::buildMipmapped(uint32_t width, uint32_t height, uint32_t depth, VulkanImageFormat format, VulkanImageUsage usage, VulkanImageAspect aspect, VulkanImageLayout layout)
        {
            return new VulkanImage(device, width, height, depth, false, format, usage, aspect, layout);
        }

        VulkanImage * VulkanImageFactory::buildMipmapped(uint32_t width, uint32_t height, uint32_t depth, VulkanImageFormat format, VulkanImageUsage usage)
        {
            return buildMipmapped(width, height, depth, format, usage, VulkanImage::resolveIsDepthBuffer(format) ? VulkanImageAspect::DepthAspect : VulkanImageAspect::ColorAspect, VulkanImageLayout::Preinitialized);
        }

        VulkanImage * VulkanImageFactory::buildMipmapped(uint32_t width, uint32_t height, VulkanImageFormat format, VulkanImageUsage usage, VulkanImageAspect aspect, VulkanImageLayout layout)
        {
            return buildMipmapped(width, height, 1, format, usage, aspect, layout);
        }

        VulkanImage * VulkanImageFactory::buildMipmapped(uint32_t width, uint32_t height, VulkanImageFormat format, VulkanImageUsage usage)
        {
            return buildMipmapped(width, height, 1, format, usage, VulkanImage::resolveIsDepthBuffer(format) ? VulkanImageAspect::DepthAspect : VulkanImageAspect::ColorAspect, VulkanImageLayout::Preinitialized);
        }

        VulkanImage * VulkanImageFactory::buildMipmapped(uint32_t width, uint32_t height, VulkanImageFormat format, int usage)
        {
            return buildMipmapped(width, height, 1, format, static_cast<VulkanImageUsage>(usage), VulkanImage::resolveIsDepthBuffer(format) ? VulkanImageAspect::DepthAspect : VulkanImageAspect::ColorAspect, VulkanImageLayout::Preinitialized);
        }

        VulkanImage * VulkanImageFactory::build(std::string mediakey)
        {
            return new VulkanImage(device, media, mediakey);
        }

        VulkanImage * VulkanImageFactory::build(uint32_t width, uint32_t height, uint32_t channelCount, void * data)
        {
            return new VulkanImage(device, width, height, channelCount, data);
        }

    }
}