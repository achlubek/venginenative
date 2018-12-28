#include "stdafx.h"
#include "VulkanAttachment.h"
namespace VEngine
{
    namespace Renderer
    {

        VulkanAttachment::VulkanAttachment(ImageInterface* image, VkFormat format, VkSampleCountFlagBits samples, VkAttachmentLoadOp loadop, VkAttachmentStoreOp storeop,
            VkAttachmentLoadOp stencilloadop, VkAttachmentStoreOp stencilstoreop, VkImageLayout initialLayout, VkImageLayout finalLayout,
            VEngineAttachmentBlending blending, VEngineClearColorValue clearColor, bool clear)
            : image(image), blending(blending), clearColor(clearColor), clear(clear)
        {
            description = {};
            description.format = format;
            description.samples = samples;
            description.loadOp = loadop;
            description.storeOp = storeop;
            description.stencilLoadOp = stencilloadop;
            description.stencilStoreOp = stencilstoreop;
            description.initialLayout = initialLayout;
            description.finalLayout = finalLayout;
        }


        VulkanAttachment::~VulkanAttachment()
        {
        }

        ImageInterface * VulkanAttachment::getImage()
        {
            return image;
        }

        VkAttachmentDescription VulkanAttachment::getDescription()
        {
            return description;
        }

        VEngineAttachmentBlending VulkanAttachment::getBlending()
        {
            return blending;
        }

        VEngineClearColorValue VulkanAttachment::getClearColor()
        {
            return clearColor;
        }

        bool VulkanAttachment::isCleared()
        {
            return clear;
        }

        VkAttachmentReference VulkanAttachment::getReference(uint32_t attachment, VkImageLayout layout)
        {
            VkAttachmentReference ref = {};
            ref.attachment = attachment;
            ref.layout = layout;
            return ref;
        }

    }
}
