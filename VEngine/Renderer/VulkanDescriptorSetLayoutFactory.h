#pragma once

namespace VEngine
{
    namespace Renderer
    {
        namespace Internal
        {
            class VulkanDevice;
        }
        class VulkanDescriptorSetLayout;

        class VulkanDescriptorSetLayoutFactory
        {
        public:
            VulkanDescriptorSetLayoutFactory(Internal::VulkanDevice * device);
            ~VulkanDescriptorSetLayoutFactory();

            VulkanDescriptorSetLayout* build();

        private:
            Internal::VulkanDevice * device;
        };

    }
}