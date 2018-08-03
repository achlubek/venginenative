#include "stdafx.h"
#include "VulkanSwapChainOutputFactory.h"
#include "VulkanSwapChainOutput.h"

namespace VEngine
{
    namespace Renderer
    {
        using namespace VEngine::Renderer::Internal;


        VulkanSwapChainOutputFactory::VulkanSwapChainOutputFactory(VulkanDevice* device)
            : device(device)
        {
        }

        VulkanSwapChainOutputFactory::~VulkanSwapChainOutputFactory()
        {
        }

        VulkanSwapChainOutput * VulkanSwapChainOutputFactory::build(VulkanRenderStage * stage)
        {
            return new VulkanSwapChainOutput(device, stage);
        }

    }
}