#include "stdafx.h"
#include "VulkanRenderStageFactory.h"
#include "VulkanRenderStage.h"

namespace VEngine
{
    namespace Renderer
    {
        using namespace VEngine::Renderer::Internal;
        VulkanRenderStageFactory::VulkanRenderStageFactory(VulkanDevice* device)
            : device(device)
        {
        }

        VulkanRenderStageFactory::~VulkanRenderStageFactory()
        {
        }

        VulkanRenderStage * VulkanRenderStageFactory::build(int width, int height, std::vector<VulkanShaderModule*> shaders, std::vector<VulkanDescriptorSetLayout*> layouts, std::vector<VulkanAttachment*> outputImages)
        {
            return new VulkanRenderStage(device, width, height, shaders, layouts, outputImages);
        }
    }
}