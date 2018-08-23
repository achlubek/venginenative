#include "stdafx.h"
#include "VulkanShaderFactory.h"
#include "VulkanShaderModule.h"
#include "../FileSystem/Media.h" 

namespace VEngine
{
    namespace Renderer
    {
        using namespace VEngine::Renderer::Internal;
        using namespace VEngine::FileSystem;

        VulkanShaderFactory::VulkanShaderFactory(VulkanDevice* device, Media* media)
            : device(device), media(media)
        {
        }

        VulkanShaderFactory::~VulkanShaderFactory()
        {
        }

        VulkanShaderModule * VulkanShaderFactory::build(VulkanShaderModuleType type, std::string mediakey)
        {
            return new VulkanShaderModule(device, type, media->getPath(mediakey));
        }

    }
}