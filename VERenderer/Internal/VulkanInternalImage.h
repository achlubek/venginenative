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

    VulkanInternalImage(VulkanDevice * vulkan, uint32_t width, uint32_t height, uint32_t depth, VkFormat format, VkImageTiling tiling,
        VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImageAspectFlags aspectFlags,
        VkImageLayout initialLayout, bool iisDepthBuffer);

    VulkanInternalImage(VulkanDevice * vulkan, uint32_t width, uint32_t height, uint32_t depth, uint32_t arrayLayers, VkFormat format, VkImageTiling tiling,
        VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImageAspectFlags aspectFlags,
        VkImageLayout initialLayout, bool iisDepthBuffer);

    VulkanInternalImage(VulkanDevice * vulkan, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling,
        VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImageAspectFlags aspectFlags,
        VkImageLayout initialLayout, bool iisDepthBuffer);

    VulkanInternalImage(VkFormat format, VkImage imageHandle, VkImageView viewHandle);

    VkSampler getSampler();
    VulkanAttachment getAttachment();

    VulkanAttachmentBlending attachmentBlending;
    void generateMipMaps();
    ~VulkanInternalImage();
private:
    bool samplerCreated = false;
    void initalize();
};
