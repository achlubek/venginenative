#pragma once
#include "../Interface/Renderer/Enums.h"
#include "../Interface/Renderer/AttachmentInterface.h"

namespace VEngine
{
    namespace Renderer
    {

        class VulkanImage;

        class VulkanAttachment : public AttachmentInterface
        {
        public:
            VulkanAttachment(ImageInterface* iimage, VkFormat format, VkSampleCountFlagBits samples, VkAttachmentLoadOp loadop, VkAttachmentStoreOp storeop,
                VkAttachmentLoadOp stencilloadop, VkAttachmentStoreOp stencilstoreop, VkImageLayout initialLayout, VkImageLayout finalLayout,
                VEngineAttachmentBlending blending, VEngineClearColorValue clearColor, bool clear);
            ~VulkanAttachment();
            virtual ImageInterface* getImage() override;
            VkAttachmentDescription getDescription();
            virtual VEngineAttachmentBlending getBlending() override;
            VEngineClearColorValue getClearColor();
            VkAttachmentReference getReference(uint32_t attachment, VkImageLayout layout);
            virtual bool isCleared() override;
        private:
            ImageInterface * image;
            VkAttachmentDescription description;
            VEngineAttachmentBlending blending;
            VEngineClearColorValue clearColor;
            bool clear = true;
        };

    }
}