#include "stdafx.h" 

VulkanGenericBuffer::VulkanGenericBuffer()
{

}
VulkanGenericBuffer::VulkanGenericBuffer(VkDeviceSize s)
{
	size = s;
    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(VulkanToolkit::singleton->device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to create vertex buffer!");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(VulkanToolkit::singleton->device, buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = VulkanToolkit::singleton->findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    if (vkAllocateMemory(VulkanToolkit::singleton->device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate vertex buffer memory!");
    }

    vkBindBufferMemory(VulkanToolkit::singleton->device, buffer, bufferMemory, 0);
}

VulkanGenericBuffer::~VulkanGenericBuffer()
{
}

void VulkanGenericBuffer::map(VkDeviceSize offset, VkDeviceSize size, void ** data)
{
    vkMapMemory(VulkanToolkit::singleton->device, bufferMemory, offset, size, 0, data);
}

void VulkanGenericBuffer::unmap()
{
    vkUnmapMemory(VulkanToolkit::singleton->device, bufferMemory);
}