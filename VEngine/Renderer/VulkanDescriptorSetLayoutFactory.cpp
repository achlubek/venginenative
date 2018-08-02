#include "stdafx.h"
#include "VulkanDescriptorSetLayoutFactory.h"
#include "VulkanDescriptorSetLayout.h"


VulkanDescriptorSetLayoutFactory::VulkanDescriptorSetLayoutFactory(VulkanDevice * device)
    : device(device)
{
}

VulkanDescriptorSetLayoutFactory::~VulkanDescriptorSetLayoutFactory()
{
}

VulkanDescriptorSetLayout * VulkanDescriptorSetLayoutFactory::build()
{
    return new VulkanDescriptorSetLayout(device);
}
