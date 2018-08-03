#pragma once 
#include "Internal/VulkanSingleAllocation.h"

#include "VulkanEnums.h"
namespace VEngine
{
    namespace Renderer
    {
        namespace Internal
        {
            class VulkanDevice;
        }

        class VulkanGenericBuffer
        {
        public:
            VulkanGenericBuffer(Internal::VulkanDevice * device, VulkanBufferType type, uint64_t s);
            ~VulkanGenericBuffer();
            void map(uint64_t offset, uint64_t size, void** data);
            void unmap();

            VkBuffer getBuffer();
            uint64_t getSize();
            VulkanBufferType getType();

        private:
            Internal::VulkanDevice * device;
            VkBuffer buffer;
            Internal::VulkanSingleAllocation bufferMemory;
            uint64_t size;
            VulkanBufferType type{ VulkanBufferType::BufferTypeUniform };
        };
    }
}
