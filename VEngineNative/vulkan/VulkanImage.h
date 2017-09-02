#pragma once 
class VulkanImage
{
public:
    VkImage image;
    VkDeviceMemory imageMemory;
    VkImageView imageView;

    uint32_t width;
    uint32_t height;
    VkFormat format;
    VkImageTiling tiling;
    VkImageUsageFlags usage;
    VkMemoryPropertyFlags properties;
    VkImageAspectFlags aspectFlags;
    VkImageLayout initialLayout;
    bool isDepthBuffer;
    bool isPresentReady = false;

    VulkanImage() {}
    VulkanImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling,
        VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImageAspectFlags aspectFlags,
        VkImageLayout initialLayout, bool iisDepthBuffer);

    VulkanImage(VkFormat format, VkImage imageHandle, VkImageView viewHandle);

    VkSampler getSampler();
    VulkanAttachment getAttachment();
    ~VulkanImage();
};
