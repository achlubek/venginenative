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
        class VulkanShaderModule
        {
        public:
            VulkanShaderModule(Internal::VulkanDevice * device, VulkanShaderModuleType type, std::string path);
            ~VulkanShaderModule();
            VkShaderModule getHandle();
            VulkanShaderModuleType getType();

        private:
            VkShaderModule handle;
            Internal::VulkanDevice * device;
            VulkanShaderModuleType type;
        };
    }
}