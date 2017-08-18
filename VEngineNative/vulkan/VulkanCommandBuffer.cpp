#include "stdafx.h"

VulkanCommandBuffer::VulkanCommandBuffer( VkCommandBufferLevel level)
{
    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = VulkanToolkit::singleton->commandPool;
    allocInfo.level = level;
    allocInfo.commandBufferCount = 1;

    vkAllocateCommandBuffers(VulkanToolkit::singleton->device, &allocInfo, &handle);
}

VulkanCommandBuffer::~VulkanCommandBuffer()
{
}

void VulkanCommandBuffer::begin(VkCommandBufferUsageFlags flags)
{
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

    vkBeginCommandBuffer(handle, &beginInfo);
}

void VulkanCommandBuffer::end()
{
    vkEndCommandBuffer(handle);
}