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

            void bindImageViewSampler(int binding, VulkanImage* img);
            void bindImageStorage(int binding, VulkanImage* img);
            void bindBuffer(int binding, VulkanGenericBuffer* buffer);


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