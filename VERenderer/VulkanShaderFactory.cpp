#include "stdafx.h"
#include "VulkanShaderFactory.h"
#include "VulkanShaderModule.h"
#include "Media.h"


VulkanShaderFactory::VulkanShaderFactory(VulkanToolkit* vulkan)
    : vulkan(vulkan)
{
}


VulkanShaderFactory::~VulkanShaderFactory()
{
}

VulkanShaderModule * VulkanShaderFactory::build(VulkanShaderModuleType type, std::string mediakey)
{
    return new VulkanShaderModule(vulkan, Media::getPath(mediakey));
}
