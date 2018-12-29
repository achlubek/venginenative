#pragma once
#include "../Interface/Renderer/Enums.h"
#include "../Interface/Renderer/ShaderModuleInterface.h"

namespace VEngine
{
    namespace Renderer
    {
        namespace Internal
        {
            class VulkanDevice;
        }
        class VulkanShaderModule : public ShaderModuleInterface
        {
        public:
            VulkanShaderModule(Internal::VulkanDevice * device, VEngineShaderModuleType type, std::string path);
            ~VulkanShaderModule();
            VkShaderModule getHandle();
            virtual VEngineShaderModuleType getType() override;

        private:
            VkShaderModule handle;
            Internal::VulkanDevice * device;
            VEngineShaderModuleType type;
        };
    }
}