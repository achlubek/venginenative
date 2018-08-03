#pragma once
namespace VEngine
{
    namespace Renderer
    {
        namespace Internal
        {
            class VulkanDevice;
        }

        class VulkanComputeStage;
        class VulkanShaderModule;
        class VulkanDescriptorSetLayout;

        class VulkanComputeStageFactory
        {
        public:
            VulkanComputeStageFactory(Internal::VulkanDevice* device);
            ~VulkanComputeStageFactory();

            VulkanComputeStage* build(VulkanShaderModule* shader, std::vector<VulkanDescriptorSetLayout*> layouts);
        private:
            Internal::VulkanDevice * device;
        };

    }
}