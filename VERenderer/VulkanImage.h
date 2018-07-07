#pragma once 
class VulkanToolkit;
#include "VulkanSingleAllocation.h"
class VulkanImage
{
public:
    VulkanToolkit * vulkan;
    VkImage image;
    VulkanSingleAllocation imageMemory;
    VkImageView imageView;

    uint32_t width;
    uint32_t height;
    uint32_t depth;
    uint32_t arrayLayers;
    VkFormat format;
    VkImageTiling tiling;
    VkImageUsageFlags usage;
    VkMemoryPropertyFlags properties;
    VkImageAspectFlags aspectFlags;
    VkImageLayout initialLayout;
    VkSampler sampler;
    bool isDepthBuffer;
    bool isPresentReady = false;
    bool clear = true;
    VkClearColorValue clearColor;

    VulkanImage(VulkanToolkit * vulkan, uint32_t width, uint32_t height, uint32_t depth, VkFormat format, VkImageTiling tiling,
        VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImageAspectFlags aspectFlags,
        VkImageLayout initialLayout, bool iisDepthBuffer);

    VulkanImage(VulkanToolkit * vulkan, uint32_t width, uint32_t height, uint32_t depth, uint32_t arrayLayers, VkFormat format, VkImageTiling tiling,
        VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImageAspectFlags aspectFlags,
        VkImageLayout initialLayout, bool iisDepthBuffer);

    VulkanImage(VulkanToolkit * vulkan, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling,
        VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImageAspectFlags aspectFlags,
        VkImageLayout initialLayout, bool iisDepthBuffer);

    VulkanImage(VkFormat format, VkImage imageHandle, VkImageView viewHandle);

    VkSampler getSampler();
    VulkanAttachment getAttachment();

    VulkanAttachmentBlending attachmentBlending;
    void generateMipMaps();
    ~VulkanImage();
private:
    bool samplerCreated = false;
    void initalize();
};
