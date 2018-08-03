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
        class VulkanDescriptorSet;

        class VulkanDescriptorSetLayout
        {
        public:
            VulkanDescriptorSetLayout(Internal::VulkanDevice * device);
            ~VulkanDescriptorSetLayout();

            void addField(VulkanDescriptorSetFieldType fieldType, VulkanDescriptorSetFieldStage fieldAccesibility);

            VulkanDescriptorSet* generateDescriptorSet();
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