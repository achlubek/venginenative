#include "stdafx.h"
#include "VulkanShaderFactory.h"
#include "VulkanShaderModule.h"
#include "../Media/Media.h" 


VulkanShaderFactory::VulkanShaderFactory(VulkanDevice* device)
    : device(device)
{
}

VulkanShaderFactory::~VulkanShaderFactory()
{
}

VulkanShaderModule * VulkanShaderFactory::build(VulkanShaderModuleType type, std::string mediakey)
{
    return new VulkanShaderModule(device, type, Media::getPath(mediakey));
}
