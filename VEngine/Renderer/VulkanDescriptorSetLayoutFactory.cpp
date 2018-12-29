#include "stdafx.h"
#include "VulkanDescriptorSetLayoutFactory.h"
#include "VulkanDescriptorSetLayout.h"

namespace VEngine
{
    namespace Renderer
    {
        using namespace VEngine::Renderer::Internal;

        VulkanDescriptorSetLayoutFactory::VulkanDescriptorSetLayoutFactory(VulkanDevice * device)
            : device(device)
        {
        }

        VulkanDescriptorSetLayoutFactory::~VulkanDescriptorSetLayoutFactory()
        {
        }

        DescriptorSetLayoutInterface * VulkanDescriptorSetLayoutFactory::build()
        {
            return new VulkanDescriptorSetLayout(device);
        }

    }
}