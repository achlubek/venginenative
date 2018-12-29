#include "stdafx.h"
#include "VulkanSwapChainOutputFactory.h"
#include "VulkanSwapChainOutput.h"

namespace VEngine
{
    namespace Renderer
    {
        using namespace VEngine::Renderer::Internal;


        VulkanSwapChainOutputFactory::VulkanSwapChainOutputFactory(VulkanDevice* device, SemaphoreFactoryInterface* semaphoreFactory)
            : device(device), semaphoreFactory(semaphoreFactory)
        {
        }

        VulkanSwapChainOutputFactory::~VulkanSwapChainOutputFactory()
        {
        }

        SwapChainOutputInterface * VulkanSwapChainOutputFactory::build(RenderStageInterface * stage)
        {
            return new VulkanSwapChainOutput(device, semaphoreFactory, stage);
        }

    }
}