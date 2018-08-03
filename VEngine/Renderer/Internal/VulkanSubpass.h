#pragma once

namespace VEngine
{
    namespace Renderer
    {
        class VulkanAttachment;
        namespace Internal
        {
            class VulkanSubpass
            {
            public:
                VulkanSubpass() {}
                VulkanSubpass(std::vector<VulkanAttachment*> colorattachments, std::vector<VkImageLayout> colorattachmentlayouts, VulkanAttachment* depthstencilattachment, VkImageLayout despthattachmentlayout);
                VulkanSubpass(std::vector<VulkanAttachment*> colorattachments, std::vector<VkImageLayout> colorattachmentlayouts);
                ~VulkanSubpass();

                VkSubpassDescription getHandle();
            private:
                VkSubpassDescription description;
            };

        }
    }
}