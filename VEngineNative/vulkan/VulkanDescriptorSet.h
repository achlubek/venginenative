#pragma once
class VulkanDescriptorSet
{
public:
	VkDescriptorPool pool;
	VkDescriptorSetLayout layout;
	VkDescriptorSet set;
	VulkanDescriptorSet();
	VulkanDescriptorSet(VkDescriptorPool p, VkDescriptorSetLayout l);
	~VulkanDescriptorSet();

	std::vector<VkWriteDescriptorSet> writes;

	void bindImageViewSampler(int binding, VulkanImage img);
	void bindUniformBuffer(int binding, VulkanGenericBuffer buffer);
	void bindStorageBuffer(int binding, VulkanGenericBuffer buffer);

	void update();
};

