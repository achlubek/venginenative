#pragma once 
class VulkanDevice;
#include "VulkanAttachment.h"
#include "VulkanSingleAllocation.h"
class VulkanInternalImage
{
public:
    VulkanInternalImage(VulkanDevice * vulkan, uint32_t width, uint32_t height, uint32_t depth, VkFormat format, VkImageTiling tiling,
        VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImageAspectFlags aspectFlags,
        VkImageLayout initialLayout);

    VulkanInternalImage(VulkanDevice * device, VkFormat format, VkImage imageHandle, VkImageView viewHandle);

    ~VulkanInternalImage();

    VkSampler getSampler();
    VkImageView getImageView();

private:
    VulkanDevice * device;
    VkImage image;
    VulkanSingleAllocation imageMemory;
    VkImageView imageView;

    uint32_t width;
    uint32_t height;
    uint32_t depth;
    VkFormat format;
    VkImageTiling tiling;
    VkImageUsageFlags usage;
    VkMemoryPropertyFlags properties;
    VkImageAspectFlags aspectFlags;
    VkImageLayout initialLayout;
    VkSampler sampler;
    bool samplerCreated = false;
    void initalize();
};
