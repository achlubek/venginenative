#include "stdafx.h" 
#include "VulkanGenericBuffer.h" 
#include "Internal/VulkanDevice.h" 
#include "Internal/VulkanMemoryManager.h" 
#include "Internal/VulkanMemoryChunk.h" 

namespace VEngine
{
    namespace Renderer
    {
        using namespace VEngine::Renderer::Internal;

        VulkanGenericBuffer::VulkanGenericBuffer(VulkanDevice * device, VEngineBufferType type, uint64_t s)
            : device(device), type(type)
        {
            VkBufferUsageFlagBits usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
            if (type == VEngineBufferType::BufferTypeUniform) {
                usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
            }
            if (type == VEngineBufferType::BufferTypeStorage) {
                usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
            }
            if (type == VEngineBufferType::BufferTypeTransferDestination) {
                usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT;
            }
            if (type == VEngineBufferType::BufferTypeTransferSource) {
                usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
            }
            size = s;
            VkBufferCreateInfo bufferInfo = {};
            bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            bufferInfo.size = size;
            bufferInfo.usage = usage;
            bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

            if (vkCreateBuffer(device->getDevice(), &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
                throw std::runtime_error("failed to create buffer!");
            }

            VkMemoryRequirements memRequirements;
            vkGetBufferMemoryRequirements(device->getDevice(), buffer, &memRequirements);

            bufferMemory = device->getMemoryManager()->bindBufferMemory(
                device->findMemoryType(memRequirements.memoryTypeBits,
                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT), buffer, memRequirements.size);
        }

        VulkanGenericBuffer::~VulkanGenericBuffer()
        {
            vkDestroyBuffer(device->getDevice(), buffer, nullptr);
            bufferMemory.free();
        }

        void VulkanGenericBuffer::map(uint64_t offset, uint64_t size, void ** data)
        {
            bufferMemory.map(offset, size, data);
        }

        void VulkanGenericBuffer::unmap()
        {
            bufferMemory.unmap();
        }

        VkBuffer VulkanGenericBuffer::getBuffer()
        {
            return buffer;
        }

        uint64_t VulkanGenericBuffer::getSize()
        {
            return size;
        }

        VEngineBufferType VulkanGenericBuffer::getType()
        {
            return type;
        }

    }
}