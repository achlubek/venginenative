#pragma once 
#include "../Interface/Renderer/Enums.h"
#include "../Interface/Renderer/GenericBufferInterface.h"
#include "Internal/VulkanSingleAllocation.h"
namespace VEngine
{
    namespace Renderer
    {
        namespace Internal
        {
            class VulkanDevice;
        }

        class VulkanGenericBuffer : public GenericBufferInterface
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
