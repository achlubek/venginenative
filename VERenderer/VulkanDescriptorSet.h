#pragma once
class VulkanToolkit;
class VulkanDescriptorSet
{
public:
    VulkanToolkit * vulkan;
    VkDescriptorPool pool;
    VkDescriptorSetLayout layout;
    VkDescriptorSet set; 
    VulkanDescriptorSet(VulkanToolkit * vulkan, VkDescriptorPool p, VkDescriptorSetLayout l);
    ~VulkanDescriptorSet();

    std::vector<VkWriteDescriptorSet> writes;

    void bindImageViewSampler(int binding, VulkanImage* img);
    void bindImageStorage(int binding, VulkanImage* img);
    void bindUniformBuffer(int binding, VulkanGenericBuffer* buffer);
    void bindStorageBuffer(int binding, VulkanGenericBuffer* buffer);

    void update();
};

