#include "stdafx.h"
#include "VulkanBufferFactory.h"
#include "VulkanGenericBuffer.h"


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
