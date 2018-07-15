#pragma once
class VulkanDevice;
class VulkanImage;
#include "VulkanEnums.h"
class VulkanImageFactory
{
public:
    VulkanImageFactory(VulkanDevice* device);
    ~VulkanImageFactory();

    VulkanImage* build(uint32_t width, uint32_t height, uint32_t depth,
        VulkanImageFormat format, VulkanImageUsage usage, VulkanImageAspect aspect, VulkanImageLayout layout);

    VulkanImage* build(uint32_t width, uint32_t height, uint32_t depth,
        VulkanImageFormat format, VulkanImageUsage usage);

    VulkanImage* build(uint32_t width, uint32_t height,
        VulkanImageFormat format, VulkanImageUsage usage, VulkanImageAspect aspect, VulkanImageLayout layout);

    VulkanImage* build(uint32_t width, uint32_t height,
        VulkanImageFormat format, VulkanImageUsage usage);

    VulkanImage* build(uint32_t width, uint32_t height, VulkanImageFormat format, int usage);

    VulkanImage* build(std::string mediakey);

    VulkanImage* build(uint32_t width, uint32_t height, uint32_t channelCount, void * data);


private:
    VulkanDevice * device;
};

