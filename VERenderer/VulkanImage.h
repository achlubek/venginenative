#pragma once 
class VulkanDevice;
#include "Internal/VulkanSingleAllocation.h"
class VulkanInternalImage;
class VulkanAttachment;
#include "VulkanAttachment.h" 

#include "VulkanEnums.h"

class VulkanImage
{
public:
    VulkanImage(VulkanDevice* device, uint32_t width, uint32_t height, uint32_t depth, 
        VulkanImageFormat format, VulkanImageUsage usage, VulkanImageAspect aspect, VulkanImageLayout layout);
    VulkanImage(VulkanDevice* device, VulkanInternalImage* internalImage, VkFormat internalFormat);
    VulkanImage(VulkanDevice* device, std::string mediakey);
    ~VulkanImage();
    // todo abstarct out VkClearColorValue
    VulkanAttachment* getAttachment(VulkanAttachmentBlending blending, bool clear, VkClearColorValue clearColor, bool forPresent);
    bool isDepthBuffer();
    VkSampler getSampler();
    VkImageView getImageView();
    static bool resolveIsDepthBuffer(VulkanImageFormat format);
private:
    VkFormat resolveFormat(VulkanImageFormat format);
    VulkanImageFormat reverseResolveFormat(VkFormat format);
    VulkanDevice * device;
    VulkanInternalImage * internalImage;
    VulkanImageFormat format;
};
