#pragma once
#include "../Interface/Renderer/SemaphoreInterface.h"

namespace VEngine
{
    namespace Renderer
    {
        namespace Internal {
            class VulkanDevice;
        }

        class VulkanSemaphore : public SemaphoreInterface
        {
        public:
            VulkanSemaphore(Internal::VulkanDevice* device);
            ~VulkanSemaphore();
            VkSemaphore getHandle();
            VkSemaphore* getHandlePointer();
        private:
            Internal::VulkanDevice* device;
            VkSemaphore handle;
        };
    }
}
