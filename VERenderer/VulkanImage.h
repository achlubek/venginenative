#pragma once 
class VulkanToolkit;
#include "VulkanMemoryChunk.h"
class VulkanImage
{
public:
    VulkanToolkit * vulkan;
    VkImage image;
    VulkanSingleAllocation imageMemory;
    VkImageView imageView;

    uint32_t width;
    uint32_t height;
    VkFormat format;
    VkImageTiling tiling;
    VkImageUsageFlags usage;
    VkMemoryPropertyFlags properties;
    VkImageAspectFlags aspectFlags;
    VkImageLayout initialLayout;
    VkSampler sampler;
    bool isDepthBuffer;
    bool isPresentReady = false;
     
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
};
