#pragma once 
#include "Internal/VulkanSingleAllocation.h"
#include "VulkanEnums.h"

namespace VEngine
{
    namespace Renderer
    {
        namespace Internal
        {
            class VulkanDevice;
            class VulkanInternalImage;
        }
        class VulkanAttachment;

        class VulkanImage
        {
        public:
            VulkanImage(Internal::VulkanDevice* device, uint32_t width, uint32_t height, uint32_t depth,
                VulkanImageFormat format, VulkanImageUsage usage, VulkanImageAspect aspect, VulkanImageLayout layout);
            VulkanImage(Internal::VulkanDevice* device, Internal::VulkanInternalImage* internalImage, VkFormat internalFormat);
            VulkanImage(Internal::VulkanDevice* device, std::string mediakey);
            VulkanImage(Internal::VulkanDevice* device, uint32_t width, uint32_t height, uint32_t channelCount, void * data);
            ~VulkanImage();
            // todo abstract out VkClearColorValue
            VulkanAttachment* getAttachment(VulkanAttachmentBlending blending, bool clear, VkClearColorValue clearColor, bool forPresent);
            VulkanAttachment* getAttachment(VulkanAttachmentBlending blending, bool clear, VkClearColorValue clearColor);
            VulkanAttachment* getAttachment(VulkanAttachmentBlending blending, bool clear);
            VulkanAttachment* getAttachment(VulkanAttachmentBlending blending, VkClearColorValue clearColor);
            VulkanAttachment* getAttachment(VulkanAttachmentBlending blending);
            bool isDepthBuffer();
            VkSampler getSampler();
            VkImageView getImageView();
            static bool resolveIsDepthBuffer(VulkanImageFormat format);
        private:
            VkFormat resolveFormat(VulkanImageFormat format);
            VulkanImageFormat reverseResolveFormat(VkFormat format);
            Internal::VulkanDevice * device;
            Internal::VulkanInternalImage * internalImage;
            VulkanImageFormat format;
        };
    }
}
