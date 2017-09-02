#include "stdafx.h"
#include "VulkanDescriptorSetLayout.h"


VulkanDescriptorSetLayout::VulkanDescriptorSetLayout()
{
    bindings = {};
    descriptorPools = {};
    generateNewPool();
}


VulkanDescriptorSetLayout::~VulkanDescriptorSetLayout()
{
}

void VulkanDescriptorSetLayout::addField(uint32_t binding, VkDescriptorType type, VkShaderStageFlags shaderstageflags)
{
    VkDescriptorSetLayoutBinding b = {};
    b.binding = binding;
    b.descriptorType = type;
    b.descriptorCount = 1;
    b.stageFlags = shaderstageflags;
    b.pImmutableSamplers = nullptr;
    bindings.push_back(b);
}

void VulkanDescriptorSetLayout::compile()
{
    VkDescriptorSetLayoutCreateInfo layoutInfo = {};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    layoutInfo.pBindings = bindings.data();

    vkCreateDescriptorSetLayout(VulkanToolkit::singleton->device, &layoutInfo, nullptr, &layout);
}

VulkanDescriptorSet VulkanDescriptorSetLayout::generateDescriptorSet()
{
    allocationCounter++;
    if (allocationCounter > 10) {
        generateNewPool();
        allocationCounter = 0;
    }
    VkDescriptorPool pool = descriptorPools[descriptorPools.size() - 1];

    return VulkanDescriptorSet(pool, layout);
}

void VulkanDescriptorSetLayout::generateNewPool()
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
