#pragma once
#include "VulkanEnums.h"
namespace VEngine
{
    namespace Renderer
    {

        class VulkanImage;

        class VulkanAttachment
        {
        public:
            VulkanAttachment(VulkanImage* iimage, VkFormat format, VkSampleCountFlagBits samples, VkAttachmentLoadOp loadop, VkAttachmentStoreOp storeop,
                VkAttachmentLoadOp stencilloadop, VkAttachmentStoreOp stencilstoreop, VkImageLayout initialLayout, VkImageLayout finalLayout,
                VulkanAttachmentBlending blending, VkClearColorValue clearColor, bool clear);
            ~VulkanAttachment();
            VulkanImage* getImage();
            VkAttachmentDescription getDescription();
            VulkanAttachmentBlending getBlending();
            VkClearColorValue getClearColor();
            VkAttachmentReference getReference(uint32_t attachment, VkImageLayout layout);
            bool isCleared();
        private:
            VulkanImage * image;
            VkAttachmentDescription description;
            VulkanAttachmentBlending blending;
            VkClearColorValue clearColor;
            bool clear = true;
        };

    }
}