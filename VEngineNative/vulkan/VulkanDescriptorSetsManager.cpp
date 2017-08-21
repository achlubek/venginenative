#include "stdafx.h" 

VulkanDescriptorSetsManager::VulkanDescriptorSetsManager()
{
	createLayoutPostProcessing();
	createLayoutMesh3d();
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
	poolSizes[0].descriptorCount = 100;
	poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[1].descriptorCount = 100;

	VkDescriptorPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = 10;
	vkCreateDescriptorPool(VulkanToolkit::singleton->device, &poolInfo, nullptr, &pool);

	descriptorPools.push_back(pool);
}

void VulkanDescriptorSetsManager::createLayoutMesh3d()
{
	VkDescriptorSetLayoutBinding globalHighFrequencyUBO = {};
	globalHighFrequencyUBO.binding = 0;
	globalHighFrequencyUBO.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	globalHighFrequencyUBO.descriptorCount = 1;
	globalHighFrequencyUBO.stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS;
	globalHighFrequencyUBO.pImmutableSamplers = nullptr; // Optional

	VkDescriptorSetLayoutBinding globalLowFrequencyUBO = {};
	globalLowFrequencyUBO.binding = 1;
	globalLowFrequencyUBO.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	globalLowFrequencyUBO.descriptorCount = 1;
	globalLowFrequencyUBO.stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS;
	globalLowFrequencyUBO.pImmutableSamplers = nullptr; // Optional

	VkDescriptorSetLayoutBinding meshInstanceData1UBO = {};
	meshInstanceData1UBO.binding = 2;
	meshInstanceData1UBO.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	meshInstanceData1UBO.descriptorCount = 1;
	meshInstanceData1UBO.stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS;
	meshInstanceData1UBO.pImmutableSamplers = nullptr; // Optional

	VkDescriptorSetLayoutBinding meshInstanceData2UBO = {};
	meshInstanceData2UBO.binding = 3;
	meshInstanceData2UBO.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	meshInstanceData2UBO.descriptorCount = 1;
	meshInstanceData2UBO.stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS;
	meshInstanceData2UBO.pImmutableSamplers = nullptr; // Optional

	VkDescriptorSetLayoutBinding meshInstanceData3UBO = {};
	meshInstanceData3UBO.binding = 4;
	meshInstanceData3UBO.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	meshInstanceData3UBO.descriptorCount = 1;
	meshInstanceData3UBO.stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS;
	meshInstanceData3UBO.pImmutableSamplers = nullptr; // Optional

	VkDescriptorSetLayoutBinding materialDataUBO = {};
	materialDataUBO.binding = 5;
	materialDataUBO.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	materialDataUBO.descriptorCount = 1;
	materialDataUBO.stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS;
	materialDataUBO.pImmutableSamplers = nullptr; // Optional

	VkDescriptorSetLayoutBinding diffuseTextureBinding = {};
	diffuseTextureBinding.binding = 6;
	diffuseTextureBinding.descriptorCount = 1;
	diffuseTextureBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	diffuseTextureBinding.pImmutableSamplers = nullptr;
	diffuseTextureBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutBinding normalTextureBinding = {};
	normalTextureBinding.binding = 7;
	normalTextureBinding.descriptorCount = 1;
	normalTextureBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	normalTextureBinding.pImmutableSamplers = nullptr;
	normalTextureBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	std::vector<VkDescriptorSetLayoutBinding> bindings = {
		globalHighFrequencyUBO, globalLowFrequencyUBO, meshInstanceData1UBO , meshInstanceData2UBO,
		materialDataUBO, meshInstanceData3UBO, diffuseTextureBinding , normalTextureBinding };
	VkDescriptorSetLayoutCreateInfo layoutInfo = {};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	layoutInfo.pBindings = bindings.data();

	vkCreateDescriptorSetLayout(VulkanToolkit::singleton->device, &layoutInfo, nullptr, &mesh3dLayout);
}

void VulkanDescriptorSetsManager::createLayoutPostProcessing()
{
	VkDescriptorSetLayoutBinding globalHighFrequencyUBO = {};
	globalHighFrequencyUBO.binding = 0;
	globalHighFrequencyUBO.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	globalHighFrequencyUBO.descriptorCount = 1;
	globalHighFrequencyUBO.stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS;
	globalHighFrequencyUBO.pImmutableSamplers = nullptr; // Optional

	VkDescriptorSetLayoutBinding globalLowFrequencyUBO = {};
	globalLowFrequencyUBO.binding = 1;
	globalLowFrequencyUBO.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	globalLowFrequencyUBO.descriptorCount = 1;
	globalLowFrequencyUBO.stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS;
	globalLowFrequencyUBO.pImmutableSamplers = nullptr; // Optional

	VkDescriptorSetLayoutBinding inputTextureBinding = {};
	inputTextureBinding.binding = 2;
	inputTextureBinding.descriptorCount = 1;
	inputTextureBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	inputTextureBinding.pImmutableSamplers = nullptr;
	inputTextureBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	std::vector<VkDescriptorSetLayoutBinding> bindings = {
		globalHighFrequencyUBO, globalLowFrequencyUBO, inputTextureBinding };
	VkDescriptorSetLayoutCreateInfo layoutInfo = {};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	layoutInfo.pBindings = bindings.data();

	vkCreateDescriptorSetLayout(VulkanToolkit::singleton->device, &layoutInfo, nullptr, &ppLayout);
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


VulkanDescriptorSet VulkanDescriptorSetsManager::generatePostProcessingDescriptorSet()
{
	allocationCounter++;
	if (allocationCounter > 10) {
		generateNewPool();
		allocationCounter = 0;
	}
	VkDescriptorPool pool = descriptorPools[descriptorPools.size() - 1];

	return VulkanDescriptorSet(pool, ppLayout);
}
