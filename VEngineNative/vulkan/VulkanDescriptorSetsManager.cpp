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
    std::array<VkDescriptorPoolSize, 3> poolSizes = {};
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = 1000;
    poolSizes[1].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    poolSizes[1].descriptorCount = 1000;
    poolSizes[2].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[2].descriptorCount = 1000;

    VkDescriptorPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = 100;
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

    VkDescriptorSetLayoutBinding meshInstanceDataUBO = {};
    meshInstanceDataUBO.binding = 2;
    meshInstanceDataUBO.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    meshInstanceDataUBO.descriptorCount = 1;
    meshInstanceDataUBO.stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS;
    meshInstanceDataUBO.pImmutableSamplers = nullptr; // Optional

    VkDescriptorSetLayoutBinding materialDataUBO = {};
    materialDataUBO.binding = 3;
    materialDataUBO.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    materialDataUBO.descriptorCount = 1;
    materialDataUBO.stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS;
    materialDataUBO.pImmutableSamplers = nullptr; // Optional

    VkDescriptorSetLayoutBinding diffuseTextureBinding = {};
    diffuseTextureBinding.binding = 4;
    diffuseTextureBinding.descriptorCount = 1;
    diffuseTextureBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    diffuseTextureBinding.pImmutableSamplers = nullptr;
    diffuseTextureBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    VkDescriptorSetLayoutBinding normalTextureBinding = {};
    normalTextureBinding.binding = 5;
    normalTextureBinding.descriptorCount = 1;
    normalTextureBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    normalTextureBinding.pImmutableSamplers = nullptr;
    normalTextureBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    VkDescriptorSetLayoutBinding bumpTextureBinding = {};
    bumpTextureBinding.binding = 6;
    bumpTextureBinding.descriptorCount = 1;
    bumpTextureBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    bumpTextureBinding.pImmutableSamplers = nullptr;
    bumpTextureBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    VkDescriptorSetLayoutBinding roughnessTextureBinding = {};
    roughnessTextureBinding.binding = 7;
    roughnessTextureBinding.descriptorCount = 1;
    roughnessTextureBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    roughnessTextureBinding.pImmutableSamplers = nullptr;
    roughnessTextureBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    VkDescriptorSetLayoutBinding metalnessTextureBinding = {};
    metalnessTextureBinding.binding = 8;
    metalnessTextureBinding.descriptorCount = 1;
    metalnessTextureBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    metalnessTextureBinding.pImmutableSamplers = nullptr;
    metalnessTextureBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;


    std::vector<VkDescriptorSetLayoutBinding> bindings = {
        globalHighFrequencyUBO, globalLowFrequencyUBO, meshInstanceDataUBO,
        materialDataUBO, diffuseTextureBinding , normalTextureBinding,
        bumpTextureBinding, roughnessTextureBinding, metalnessTextureBinding };
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

    VkDescriptorSetLayoutBinding diffuseTextureBinding = {};
    diffuseTextureBinding.binding = 2;
    diffuseTextureBinding.descriptorCount = 1;
    diffuseTextureBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    diffuseTextureBinding.pImmutableSamplers = nullptr;
    diffuseTextureBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    VkDescriptorSetLayoutBinding normalTextureBinding = {};
    normalTextureBinding.binding = 3;
    normalTextureBinding.descriptorCount = 1;
    normalTextureBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    normalTextureBinding.pImmutableSamplers = nullptr;
    normalTextureBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    VkDescriptorSetLayoutBinding distanceTextureBinding = {};
    distanceTextureBinding.binding = 4;
    distanceTextureBinding.descriptorCount = 1;
    distanceTextureBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    distanceTextureBinding.pImmutableSamplers = nullptr;
    distanceTextureBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    VkDescriptorSetLayoutBinding ambientTextureBinding = {};
    ambientTextureBinding.binding = 5;
    ambientTextureBinding.descriptorCount = 1;
    ambientTextureBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    ambientTextureBinding.pImmutableSamplers = nullptr;
    ambientTextureBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    std::vector<VkDescriptorSetLayoutBinding> bindings = {
        globalHighFrequencyUBO, globalLowFrequencyUBO, diffuseTextureBinding, normalTextureBinding, distanceTextureBinding, ambientTextureBinding
    };
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
