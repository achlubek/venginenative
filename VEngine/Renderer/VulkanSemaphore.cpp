#include "stdafx.h"
#include "VulkanSemaphore.h"


VulkanSemaphore::VulkanSemaphore()
{
    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    vkCreateSemaphore(device->getDevice(), &semaphoreInfo, nullptr, &handle);
}


VulkanSemaphore::~VulkanSemaphore()
{
    vkDestroySemaphore(device->getDevice(), handle, nullptr);
}

