#include "stdafx.h"
#include "VulkanComputeStageFactory.h"
#include "VulkanComputeStage.h"


VulkanComputeStageFactory::VulkanComputeStageFactory(VulkanDevice* device)
    : device(device)
{
}

VulkanComputeStageFactory::~VulkanComputeStageFactory()
{
}

VulkanComputeStage * VulkanComputeStageFactory::build(VulkanShaderModule * shader, std::vector<VulkanDescriptorSetLayout*> layouts)
{
    return new VulkanComputeStage(device, shader, layouts);
}
