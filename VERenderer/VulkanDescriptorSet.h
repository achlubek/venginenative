#pragma once
class VulkanDevice;
class VulkanImage;
class VulkanGenericBuffer; 

class VulkanDescriptorSet
{
public:
    VulkanDescriptorSet(VulkanDevice * device, VkDescriptorPool p, VkDescriptorSetLayout l);
    ~VulkanDescriptorSet();

    void bindImageViewSampler(int binding, VulkanImage* img);
    void bindImageStorage(int binding, VulkanImage* img);
    void bindBuffer(int binding, VulkanGenericBuffer* buffer);

    void update();

    VkDescriptorSet getSet();

private:
    VulkanDevice * device;
    VkDescriptorPool pool;
    VkDescriptorSet set;
    std::vector<VkWriteDescriptorSet> writes;
};

