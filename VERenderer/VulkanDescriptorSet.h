#pragma once
class VulkanDevice;
class VulkanImage;
class VulkanGenericBuffer; 
#include <vulkan.h>

class VulkanDescriptorSet
{
public:
    VulkanDescriptorSet(VulkanDevice * device, VkDescriptorPool p, VkDescriptorSetLayout l);
    ~VulkanDescriptorSet();

    void bindImageViewSampler(int binding, VulkanImage* img);
    void bindImageStorage(int binding, VulkanImage* img);
    void bindUniformBuffer(int binding, VulkanGenericBuffer* buffer);
    void bindStorageBuffer(int binding, VulkanGenericBuffer* buffer);

    void update();

    VkDescriptorSet getSet();

private:
    VulkanDevice * device;
    VkDescriptorPool pool;
   // VkDescriptorSetLayout layout;
    VkDescriptorSet set;
    std::vector<VkWriteDescriptorSet> writes;
};

