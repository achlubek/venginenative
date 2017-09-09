#pragma once 
class VulkanToolkit;
class VulkanImage
{
public:
    VulkanToolkit * vulkan;
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
    VkSampler sampler;
    bool isDepthBuffer;
    bool isPresentReady = false;
     
    VulkanImage(VulkanToolkit * vulkan, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling,
        VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImageAspectFlags aspectFlags,
        VkImageLayout initialLayout, bool iisDepthBuffer);

    VulkanImage(VkFormat format, VkImage imageHandle, VkImageView viewHandle);

    VkSampler getSampler();
    VulkanAttachment getAttachment();
    ~VulkanImage();
private:
    bool samplerCreated = false;
};
