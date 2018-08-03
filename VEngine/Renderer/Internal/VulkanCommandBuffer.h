#pragma once

namespace VEngine
{
    namespace Renderer
    {
        namespace Internal
        {

            class VulkanDevice;
            class VulkanCommandBuffer
            {
            public:
                VulkanCommandBuffer(VulkanDevice * device, VkCommandBufferLevel level);
                ~VulkanCommandBuffer();
                void begin(VkCommandBufferUsageFlags flags);
                void end();
                VkCommandBuffer getHandle();

            private:
                VkCommandBuffer handle;
                VulkanDevice * device;
            };

        }
    }
}