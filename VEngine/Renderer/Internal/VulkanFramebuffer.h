#pragma once

namespace VEngine
{
    namespace Renderer
    {
        namespace Internal
        {

            class VulkanDevice;
            class VulkanRenderPass;
            class VulkanFramebuffer
            {
            public:
                VulkanFramebuffer(VulkanDevice * device, uint32_t width, uint32_t height, VulkanRenderPass* renderPass, std::vector<VkImageView> attachments);
                ~VulkanFramebuffer();
                VkFramebuffer getHandle();
            private:
                VkFramebuffer handle;
                VulkanDevice * device;
            };

        }
    }
}