#pragma once 
#include "../Interface/Renderer/Enums.h"
#include "../Interface/Renderer/ImageInterface.h"
#include "Internal/VulkanSingleAllocation.h"

namespace VEngine
{
    namespace FileSystem
    {
        class MediaInterface;
    }

    namespace Renderer
    {
        namespace Internal
        {
            class VulkanDevice;
            class VulkanInternalImage;
        }
        class VulkanAttachment;

        class VulkanImage : public ImageInterface
        {
        public:
            VulkanImage(Internal::VulkanDevice* device, uint32_t width, uint32_t height, uint32_t depth, bool mipmapped,
                VEngineImageFormat format, VEngineImageUsage usage, VEngineImageAspect aspect, VEngineImageLayout layout);
            VulkanImage(Internal::VulkanDevice* device, Internal::VulkanInternalImage* internalImage, VkFormat internalFormat);
            VulkanImage(Internal::VulkanDevice* device, FileSystem::MediaInterface* media, std::string mediakey);
            VulkanImage(Internal::VulkanDevice* device, uint32_t width, uint32_t height, uint32_t channelCount, void * data);
            ~VulkanImage();
            virtual AttachmentInterface* getAttachment(VEngineAttachmentBlending blending, bool clear, VEngineClearColorValue clearColor, bool forPresent) override;
            virtual AttachmentInterface* getAttachment(VEngineAttachmentBlending blending, bool clear, VEngineClearColorValue clearColor) override;
            virtual AttachmentInterface* getAttachment(VEngineAttachmentBlending blending, bool clear) override;
            virtual AttachmentInterface* getAttachment(VEngineAttachmentBlending blending, VEngineClearColorValue clearColor) override;
            virtual AttachmentInterface* getAttachment(VEngineAttachmentBlending blending) override;
            virtual bool isDepthBuffer() override;
            virtual void regenerateMipmaps() override;
            VkSampler getSampler();
            VkImageView getImageView();
            VkImageView getFirstMipmapImageView();
        private:
            VkFormat resolveFormat(VEngineImageFormat format);
            VEngineImageFormat reverseResolveFormat(VkFormat format);
            Internal::VulkanDevice * device;
            Internal::VulkanInternalImage * internalImage;
            VEngineImageFormat format;
        };
    }
}
