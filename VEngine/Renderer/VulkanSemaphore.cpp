#include "stdafx.h"
#include "VulkanSemaphore.h"
#include "Internal/VulkanDevice.h"

namespace VEngine
{
    namespace Renderer
    {
        using namespace VEngine::Renderer::Internal;

        VulkanSemaphore::VulkanSemaphore(VulkanDevice* device)
            : device(device)
        {
            VkSemaphoreCreateInfo semaphoreInfo = {};
            semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
            vkCreateSemaphore(device->getDevice(), &semaphoreInfo, nullptr, &handle);
        }

        VulkanSemaphore::~VulkanSemaphore()
        {
            vkDestroySemaphore(device->getDevice(), handle, nullptr);
        }
        VkSemaphore VulkanSemaphore::getHandle()
        {
            return handle;
        }
        VkSemaphore * VulkanSemaphore::getHandlePointer()
        {
            return &handle;
        }
    }
}