#pragma once
#include "VulkanEnums.h"

namespace VEngine
{
    namespace FileSystem
    {
        class Media;
    }

    namespace Renderer
    {
        namespace Internal
        {
            class VulkanDevice;
        }

        class VulkanShaderModule;

        class VulkanShaderFactory
        {
        public:
            VulkanShaderFactory(Internal::VulkanDevice* device, FileSystem::Media* media);
            ~VulkanShaderFactory();

            VulkanShaderModule* build(VulkanShaderModuleType type, std::string mediakey);
        private:
            Internal::VulkanDevice * device;
            FileSystem::Media* media;
        };

    }
}