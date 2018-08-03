#include "stdafx.h"
#include "VulkanSubpass.h"
#include "../VulkanAttachment.h"
namespace VEngine
{
    namespace Renderer
    {
        namespace Internal
        {
            VulkanSubpass::VulkanSubpass(std::vector<VulkanAttachment*> colorattachments, std::vector<VkImageLayout> colorattachmentlayouts, VulkanAttachment* depthstencilattachment, VkImageLayout despthattachmentlayout)
            {
                VkAttachmentReference* descs = new VkAttachmentReference[colorattachments.size()];
                VkAttachmentReference* depthref = new VkAttachmentReference[1];
                (*depthref) = depthstencilattachment->getReference(static_cast<uint32_t>(colorattachments.size()), despthattachmentlayout);
                for (int i = 0; i < colorattachments.size(); i++)descs[i] = colorattachments[i]->getReference(i, colorattachmentlayouts[i]);
                description = {};
                description.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
                description.colorAttachmentCount = static_cast<uint32_t>(colorattachments.size());
                description.pColorAttachments = &descs[0];
                description.pDepthStencilAttachment = depthref;
            }
            VulkanSubpass::VulkanSubpass(std::vector<VulkanAttachment*> colorattachments, std::vector<VkImageLayout> colorattachmentlayouts)
            {
                VkAttachmentReference* descs = new VkAttachmentReference[colorattachments.size()];
                for (int i = 0; i < colorattachments.size(); i++)descs[i] = colorattachments[i]->getReference(i, colorattachmentlayouts[i]);
                description = {};
                description.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
                description.colorAttachmentCount = static_cast<uint32_t>(colorattachments.size());
                description.pColorAttachments = &descs[0];
            }


            VulkanSubpass::~VulkanSubpass()
            {
            }

            VkSubpassDescription VulkanSubpass::getHandle()
            {
                return description;
            }
        }
    }
}