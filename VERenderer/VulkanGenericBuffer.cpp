#include "stdafx.h" 
#include "VulkanToolkit.h" 
#include "VulkanMemoryManager.h" 
#include "VulkanMemoryChunk.h" 

VulkanGenericBuffer::VulkanGenericBuffer(VulkanToolkit * ivulkan, VkBufferUsageFlags usage, VkDeviceSize s)
    : vulkan(ivulkan)
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

    if (vkCreateBuffer(vulkan->device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to create buffer!");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(vulkan->device, buffer, &memRequirements);
     
    bufferMemory = vulkan->memoryManager->bindBufferMemory(
        vulkan->findMemoryType(memRequirements.memoryTypeBits, 
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT), buffer, memRequirements.size);
}

VulkanGenericBuffer::~VulkanGenericBuffer()
{
    vkDestroyBuffer(vulkan->device, buffer, nullptr);
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