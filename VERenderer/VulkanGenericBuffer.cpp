#include "stdafx.h" 
#include "VulkanGenericBuffer.h" 
#include "Internal/VulkanDevice.h" 
#include "Internal/VulkanMemoryManager.h" 
#include "Internal/VulkanMemoryChunk.h" 

VulkanGenericBuffer::VulkanGenericBuffer(VulkanDevice * device, VkBufferUsageFlags usage, VkDeviceSize s)
    : device(device)
{
    if (usage == VK_BUFFER_USAGE_STORAGE_BUFFER_BIT)
    {
        type = VulkanBufferType::storage;
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

void VulkanGenericBuffer::map(VkDeviceSize offset, VkDeviceSize size, void ** data)
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
