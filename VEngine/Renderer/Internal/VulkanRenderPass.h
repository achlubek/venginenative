#pragma once

#include "VulkanSubpass.h"

namespace VEngine
{
    namespace Renderer
    {
        class VulkanAttachment;
        namespace Internal
        {
            class VulkanDevice;
            class VulkanRenderPass
            {
            public:
                VulkanRenderPass(VulkanDevice * device, std::vector<VulkanAttachment*> iattachments, std::vector<VulkanSubpass> subpasses);
                ~VulkanRenderPass();
                VkRenderPass getHandle();
                std::vector<VulkanAttachment*> getAttachments();
            private:
                VulkanDevice * device;
                VkRenderPass handle;
                std::vector<VulkanAttachment*> attachments;
            };
        }
    }
}
