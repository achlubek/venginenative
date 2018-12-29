#pragma once
#include "../Interface/Renderer/DescriptorSetInterface.h"

namespace VEngine
{
    namespace Renderer
    {
        namespace Internal
        {
            class VulkanDevice;
        }

        class VulkanDescriptorSet : public DescriptorSetInterface
        {
        public:
            VulkanDescriptorSet(Internal::VulkanDevice * device, VkDescriptorPool p, VkDescriptorSetLayout l);
            ~VulkanDescriptorSet();

            virtual void bindImageViewSampler(uint32_t binding, ImageInterface* img) override;
            virtual void bindImageStorage(uint32_t binding, ImageInterface* img) override;
            virtual void bindBuffer(uint32_t binding, GenericBufferInterface* buffer) override;

            VkDescriptorSet getSet();

        private:
            Internal::VulkanDevice * device;
            VkDescriptorPool pool;
            VkDescriptorSet set;
            std::vector<VkWriteDescriptorSet> writes;

            void update();
            bool needsUpdate = true;
        };

    }
}