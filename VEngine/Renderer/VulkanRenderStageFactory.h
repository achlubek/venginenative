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
        class VulkanRenderStage;
        class VulkanShaderModule;
        class VulkanDescriptorSetLayout;
        class VulkanAttachment;

        class VulkanRenderStageFactory
        {
        public:
            VulkanRenderStageFactory(Internal::VulkanDevice* device);
            ~VulkanRenderStageFactory();
            VulkanRenderStage* build(int width, int height,
                std::vector<VulkanShaderModule*> shaders,
                std::vector<VulkanDescriptorSetLayout*> layouts);

            VulkanRenderStage* build(int width, int height,
                std::vector<VulkanShaderModule*> shaders,
                std::vector<VulkanDescriptorSetLayout*> layouts,
                std::vector<VulkanAttachment*> outputImages);

            VulkanRenderStage* build(int width, int height,
                std::vector<VulkanShaderModule*> shaders,
                std::vector<VulkanDescriptorSetLayout*> layouts,
                VulkanCullMode cullMode);

            VulkanRenderStage* build(int width, int height,
                std::vector<VulkanShaderModule*> shaders,
                std::vector<VulkanDescriptorSetLayout*> layouts,
                std::vector<VulkanAttachment*> outputImages,
                VulkanCullMode cullMode);

        private:
            Internal::VulkanDevice * device;
        };

    }
}