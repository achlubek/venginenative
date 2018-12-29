#include "stdafx.h"
#include "VulkanComputeStageFactory.h"
#include "VulkanComputeStage.h"
namespace VEngine
{
    namespace Renderer
    {
        using namespace VEngine::Renderer::Internal;

        VulkanComputeStageFactory::VulkanComputeStageFactory(VulkanDevice* device, SemaphoreFactoryInterface* semaphoreFactory)
            : device(device), semaphoreFactory(semaphoreFactory)
        {
        }

        VulkanComputeStageFactory::~VulkanComputeStageFactory()
        {
        }

        ComputeStageInterface * VulkanComputeStageFactory::build(ShaderModuleInterface * shader, std::vector<DescriptorSetLayoutInterface*> layouts)
        {
            return new VulkanComputeStage(device, semaphoreFactory, shader, layouts);
        }

    }
}