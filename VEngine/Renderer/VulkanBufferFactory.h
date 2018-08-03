#pragma once
#include "VulkanEnums.h"

namespace VEngine
{
    namespace Renderer
    {
        namespace Internal
        {
            class VulkanDevice;
        }
        class VulkanGenericBuffer;

        class VulkanBufferFactory
        {
        public:
            VulkanBufferFactory(Internal::VulkanDevice* device);
            ~VulkanBufferFactory();

            VulkanGenericBuffer* build(VulkanBufferType type, uint64_t size);
        private:
            Internal::VulkanDevice * device;
        };

    }
}