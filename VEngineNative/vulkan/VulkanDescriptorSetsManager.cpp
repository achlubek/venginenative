#include "stdafx.h" 

VulkanDescriptorSetsManager::VulkanDescriptorSetsManager()
{
	if (VulkanToolkit::singleton == nullptr)return;

	VkDescriptorSetLayoutBinding uboLayoutBinding = {};
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS;
	uboLayoutBinding.pImmutableSamplers = nullptr; // Optional

	VkDescriptorSetLayoutBinding samplerLayoutBinding = {};
	samplerLayoutBinding.binding = 1;
	samplerLayoutBinding.descriptorCount = 1;
	samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerLayoutBinding.pImmutableSamplers = nullptr;
	samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	std::array<VkDescriptorSetLayoutBinding, 2> bindings = { uboLayoutBinding, samplerLayoutBinding };
	VkDescriptorSetLayoutCreateInfo layoutInfo = {};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	layoutInfo.pBindings = bindings.data();

	vkCreateDescriptorSetLayout(VulkanToolkit::singleton->device, &layoutInfo, nullptr, &mesh3dLayout);
	descriptorPools = {};
	generateNewPool();
}


VulkanDescriptorSetsManager::~VulkanDescriptorSetsManager()
{
}

void VulkanDescriptorSetsManager::generateNewPool()
{
	VkDescriptorPool pool;
	std::array<VkDescriptorPoolSize, 2> poolSizes = {};
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount = 10;
	poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[1].descriptorCount = 10;

	VkDescriptorPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = 10;
	vkCreateDescriptorPool(VulkanToolkit::singleton->device, &poolInfo, nullptr, &pool);

	descriptorPools.push_back(pool);
}

VulkanDescriptorSet VulkanDescriptorSetsManager::generateMesh3dDescriptorSet()
{
	allocationCounter++;
	if (allocationCounter > 10) {
		generateNewPool();
		allocationCounter = 0;
	}
	VkDescriptorPool pool = descriptorPools[descriptorPools.size() - 1];

	return VulkanDescriptorSet(pool, mesh3dLayout);
}
