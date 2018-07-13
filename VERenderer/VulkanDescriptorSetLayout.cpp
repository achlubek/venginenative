#include "stdafx.h"
#include "Internal/VulkanDevice.h"
#include "VulkanDescriptorSetLayout.h"
#include "VulkanDescriptorSet.h" 


VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(VulkanDevice * device)
    : device(device)
{
    bindings = {};
    descriptorPools = {};
    allocationCounter = 9999999;
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

    vkCreateDescriptorSetLayout(device->getDevice(), &layoutInfo, nullptr, &layout);
}

VulkanDescriptorSet* VulkanDescriptorSetLayout::generateDescriptorSet()
{
    allocationCounter++;
    if (allocationCounter > 10) {
        generateNewPool();
        allocationCounter = 0;
    }
    VkDescriptorPool pool = descriptorPools[descriptorPools.size() - 1];

    return new VulkanDescriptorSet(device, pool, layout);
}

VkDescriptorSetLayout VulkanDescriptorSetLayout::getHandle()
{
    return layout;
}

void VulkanDescriptorSetLayout::generateNewPool()
{
    VkDescriptorPool pool;
    std::array<VkDescriptorPoolSize, 4> poolSizes = {};
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = 1000;
    poolSizes[1].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    poolSizes[1].descriptorCount = 1000;
    poolSizes[2].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[2].descriptorCount = 1000;
    poolSizes[3].type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    poolSizes[3].descriptorCount = 1000;

    VkDescriptorPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = 100;
    vkCreateDescriptorPool(device->getDevice(), &poolInfo, nullptr, &pool);

    descriptorPools.push_back(pool);
}
