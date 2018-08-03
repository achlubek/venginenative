#include "stdafx.h"
#include "VulkanCommandBuffer.h"
#include "VulkanDevice.h"
namespace VEngine
{
    namespace Renderer
    {
        namespace Internal
        {
            VulkanCommandBuffer::VulkanCommandBuffer(VulkanDevice * device, VkCommandBufferLevel level)
                : device(device)
            {
                VkCommandBufferAllocateInfo allocInfo = {};
                allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
                allocInfo.commandPool = device->getCommandPool();
                allocInfo.level = level;
                allocInfo.commandBufferCount = 1;

                vkAllocateCommandBuffers(device->getDevice(), &allocInfo, &handle);
            }

            VulkanCommandBuffer::~VulkanCommandBuffer()
            {
            }

            void VulkanCommandBuffer::begin(VkCommandBufferUsageFlags flags)
            {
                VkCommandBufferBeginInfo beginInfo = {};
                beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
                beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT | flags;

                vkBeginCommandBuffer(handle, &beginInfo);
            }

            void VulkanCommandBuffer::end()
            {
                vkEndCommandBuffer(handle);
            }

            VkCommandBuffer VulkanCommandBuffer::getHandle()
            {
                return handle;
            }
        }
    }
}