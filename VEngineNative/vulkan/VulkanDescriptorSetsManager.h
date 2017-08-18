#pragma once
class VulkanDescriptorSetsManager
{
public:
	VulkanDescriptorSetsManager();
	~VulkanDescriptorSetsManager();

	int allocationCounter = 0;

	std::vector<VkDescriptorPool> descriptorPools;
	VkDescriptorSetLayout mesh3dLayout;

	void generateNewPool();

	VulkanDescriptorSet generateMesh3dDescriptorSet();
};

