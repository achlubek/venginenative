#pragma once

namespace VEngine
{
    namespace Renderer
    {
        class VulkanSemaphore
        {
        public:
            VulkanSemaphore();
            ~VulkanSemaphore();
            VkSemaphore getHandle();
        private:
            VkSemaphore handle;
        };
    }
}
