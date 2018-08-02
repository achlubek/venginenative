#include "stdafx.h"
#include "VulkanImageFactory.h"
#include "VulkanImage.h"


VulkanImageFactory::VulkanImageFactory(VulkanDevice* device)
    : device(device)
{
}

VulkanImageFactory::~VulkanImageFactory()
{
}

VulkanImage * VulkanImageFactory::build(uint32_t width, uint32_t height, uint32_t depth, VulkanImageFormat format, VulkanImageUsage usage, VulkanImageAspect aspect, VulkanImageLayout layout)
{
    return new VulkanImage(device, width, height, depth, format, usage, aspect, layout);
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

VulkanImage * VulkanImageFactory::build(std::string mediakey)
{
    return new VulkanImage(device, mediakey);
}

VulkanImage * VulkanImageFactory::build(uint32_t width, uint32_t height, uint32_t channelCount, void * data)
{
    return new VulkanImage(device, width, height, channelCount, data);
}
