#include "stdafx.h" 

VulkanGenericBuffer::VulkanGenericBuffer(VulkanToolkit * ivulkan, VkBufferUsageFlags usage, VkDeviceSize s)
    : vulkan(ivulkan)
{
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

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = vulkan->findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    if (vkAllocateMemory(vulkan->device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate buffer memory!");
    }

    vkBindBufferMemory(vulkan->device, buffer, bufferMemory, 0);
}

VulkanGenericBuffer::~VulkanGenericBuffer()
{
}

void VulkanGenericBuffer::map(VkDeviceSize offset, VkDeviceSize size, void ** data)
{
    vkMapMemory(vulkan->device, bufferMemory, offset, size, 0, data);
}

void VulkanGenericBuffer::unmap()
{
    vkUnmapMemory(vulkan->device, bufferMemory);
}