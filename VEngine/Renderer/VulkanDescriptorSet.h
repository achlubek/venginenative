#pragma once
namespace VEngine
{
    namespace Renderer
    {
        namespace Internal
        {
            class VulkanDevice;
        }
        class VulkanImage;
        class VulkanGenericBuffer;

        class VulkanDescriptorSet
        {
        public:
            VulkanDescriptorSet(Internal::VulkanDevice * device, VkDescriptorPool p, VkDescriptorSetLayout l);
            ~VulkanDescriptorSet();

            void bindImageViewSampler(uint32_t binding, VulkanImage* img);
            void bindImageStorage(uint32_t binding, VulkanImage* img);
            void bindBuffer(uint32_t binding, VulkanGenericBuffer* buffer);


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