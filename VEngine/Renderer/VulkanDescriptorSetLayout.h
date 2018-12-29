#pragma once
#include "../Interface/Renderer/Enums.h"
#include "../Interface/Renderer/DescriptorSetLayoutInterface.h"

namespace VEngine
{
    namespace Renderer
    {
        namespace Internal
        {
            class VulkanDevice;
        }

        class VulkanDescriptorSetLayout : public DescriptorSetLayoutInterface
        {
        public:
            VulkanDescriptorSetLayout(Internal::VulkanDevice * device);
            ~VulkanDescriptorSetLayout();

            virtual void addField(VEngineDescriptorSetFieldType fieldType, VEngineDescriptorSetFieldStage fieldAccesibility) override;

            virtual DescriptorSetInterface* generateDescriptorSet() override;
            VkDescriptorSetLayout getHandle();
        private:
            std::vector<VkDescriptorSetLayoutBinding> bindings;
            void generateNewPool();

            Internal::VulkanDevice * device;
            int allocationCounter = 0;

            std::vector<VkDescriptorPool> descriptorPools = {};
            VkDescriptorSetLayout layout;

            bool compiled = false;
            void compile();
        };

    }
}