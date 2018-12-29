#include "stdafx.h"
#include "VulkanSemaphoreFactory.h"
#include "VulkanSemaphore.h"

namespace VEngine
{
    namespace Renderer
    {
        using namespace VEngine::Renderer::Internal;

        VulkanSemaphoreFactory::VulkanSemaphoreFactory(VulkanDevice * device)
            : device(device)
        {
        }

        VulkanSemaphoreFactory::~VulkanSemaphoreFactory()
        {
        }

        SemaphoreInterface * VulkanSemaphoreFactory::build()
        {
            return new VulkanSemaphore(device);
        }

    }
}