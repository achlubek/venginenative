#pragma once 
class VulkanDevice;
#include "VulkanAttachment.h"
#include "VulkanSingleAllocation.h"
#include <vulkan.h>
class VulkanInternalImage
{
public:
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

    VulkanInternalImage(VulkanDevice * vulkan, uint32_t width, uint32_t height, uint32_t depth, VkFormat format, VkImageTiling tiling,
        VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImageAspectFlags aspectFlags,
        VkImageLayout initialLayout);


    VulkanInternalImage(VkFormat format, VkImage imageHandle, VkImageView viewHandle);

    VkSampler getSampler();

    ~VulkanInternalImage();
private:
    bool samplerCreated = false;
    void initalize();
};
