#pragma once
#include "../Interface/Renderer/SemaphoreFactoryInterface.h"

namespace VEngine
{
    namespace Renderer
    {
        namespace Internal
        {
            class VulkanDevice;
        }
        class SemaphoreInterface;

        class VulkanSemaphoreFactory : public SemaphoreFactoryInterface
        {
        public:
            VulkanSemaphoreFactory(Internal::VulkanDevice * device);
            ~VulkanSemaphoreFactory();

            virtual SemaphoreInterface* build() override;

        private:
            Internal::VulkanDevice * device;
        };

    }
}