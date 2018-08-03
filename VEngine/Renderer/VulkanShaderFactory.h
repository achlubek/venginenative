#pragma once
#include "VulkanEnums.h"

namespace VEngine
{
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
            VulkanShaderFactory(Internal::VulkanDevice* device);
            ~VulkanShaderFactory();

            VulkanShaderModule* build(VulkanShaderModuleType type, std::string mediakey);
        private:
            Internal::VulkanDevice * device;
        };

    }
}