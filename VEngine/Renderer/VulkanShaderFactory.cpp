#include "stdafx.h"
#include "VulkanShaderFactory.h"
#include "VulkanShaderModule.h"
#include "../Interface/FileSystem/MediaInterface.h"

namespace VEngine
{
    namespace Renderer
    {
        using namespace VEngine::Renderer::Internal;
        using namespace VEngine::FileSystem;

        VulkanShaderFactory::VulkanShaderFactory(VulkanDevice* device, MediaInterface* media)
            : device(device), media(media)
        {
        }

        VulkanShaderFactory::~VulkanShaderFactory()
        {
        }

        ShaderModuleInterface * VulkanShaderFactory::build(VEngineShaderModuleType type, std::string mediakey)
        {
            return new VulkanShaderModule(device, type, media->getPath(mediakey));
        }

    }
}