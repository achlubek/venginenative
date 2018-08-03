#include "stdafx.h"
#include "VulkanShaderFactory.h"
#include "VulkanShaderModule.h"
#include "../Media/Media.h" 

namespace VEngine
{
    namespace Renderer
    {
        using namespace VEngine::Renderer::Internal;
        using namespace VEngine::FileSystem;

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

    }
}