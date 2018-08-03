#include "stdafx.h"
#include "VulkanBufferFactory.h"
#include "VulkanGenericBuffer.h"

namespace VEngine
{
    namespace Renderer
    {
        using namespace VEngine::Renderer::Internal;

        VulkanBufferFactory::VulkanBufferFactory(VulkanDevice* device)
            : device(device)
        {
        }

        VulkanBufferFactory::~VulkanBufferFactory()
        {
        }

        VulkanGenericBuffer* VulkanBufferFactory::build(VulkanBufferType type, uint64_t size)
        {
            return new VulkanGenericBuffer(device, type, size);
        }

    }
}