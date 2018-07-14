#pragma once 
class VulkanDevice;
#include "../VulkanAttachment.h"
#include "VulkanSingleAllocation.h"

struct ImageData {
public:
    int width, height, channelCount;
    void* data;
};

class VulkanInternalImage
{
public:
    VulkanInternalImage(VulkanDevice * vulkan, uint32_t width, uint32_t height, uint32_t depth, VkFormat format, VkImageTiling tiling,
        VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImageAspectFlags aspectFlags,
        VkImageLayout initialLayout);

    VulkanInternalImage(VulkanDevice * device, VkFormat format, VkImage imageHandle, VkImageView viewHandle);

    VulkanInternalImage(VulkanDevice * device, std::string mediakey);

    VulkanInternalImage(VulkanDevice * device, uint32_t width, uint32_t height, uint32_t channelCount, void* data);

    ~VulkanInternalImage();

    VkSampler getSampler();
    VkImageView getImageView();
    VkFormat getFormat();

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


    ImageData readFileImageData(std::string path);
    void createTexture(const ImageData &img, VkFormat format);

    VkCommandBuffer beginSingleTimeCommands();
    void endSingleTimeCommands(VkCommandBuffer commandBuffer);
    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
    void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
    void transitionImageLayoutExistingCommandBuffer(VkCommandBuffer buffer, uint32_t mipmap, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout
        , VkPipelineStageFlags sourceStage, VkPipelineStageFlags destinationStage);
    void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
};
