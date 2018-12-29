#pragma once
#include "../Interface/Renderer/DescriptorSetLayoutFactoryInterface.h"

namespace VEngine
{
    namespace Renderer
    {
        namespace Internal
        {
            class VulkanDevice;
        }

        class VulkanDescriptorSetLayoutFactory : public DescriptorSetLayoutFactoryInterface
        {
        public:
            VulkanDescriptorSetLayoutFactory(Internal::VulkanDevice * device);
            ~VulkanDescriptorSetLayoutFactory();

            virtual DescriptorSetLayoutInterface* build() override;

        private:
            Internal::VulkanDevice * device;
        };

    }
}