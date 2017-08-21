#pragma once
class VulkanDescriptorSetsManager
{
public:
	VulkanDescriptorSetsManager();
	~VulkanDescriptorSetsManager();

	int allocationCounter = 0;

	std::vector<VkDescriptorPool> descriptorPools;
	VkDescriptorSetLayout mesh3dLayout;
	VkDescriptorSetLayout ppLayout;

	void generateNewPool();
	void createLayoutMesh3d();
	void createLayoutPostProcessing();

	VulkanDescriptorSet generateMesh3dDescriptorSet();
	VulkanDescriptorSet generatePostProcessingDescriptorSet();
};

