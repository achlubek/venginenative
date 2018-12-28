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

        GenericBufferInterface* VulkanBufferFactory::build(VEngineBufferType type, uint64_t size)
        {
            return new VulkanGenericBuffer(device, type, size);
        }

    }
}