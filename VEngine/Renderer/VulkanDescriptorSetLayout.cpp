#include "stdafx.h"
#include "Internal/VulkanDevice.h"
#include "VulkanDescriptorSetLayout.h"
#include "VulkanDescriptorSet.h" 

namespace VEngine
{
    namespace Renderer
    {
        using namespace VEngine::Renderer::Internal;

        VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(VulkanDevice * device)
            : device(device)
        {
            bindings = {};
            descriptorPools = {};
            allocationCounter = 9999999;
        }


        VulkanDescriptorSetLayout::~VulkanDescriptorSetLayout()
        {
            for (int i = 0; i < descriptorPools.size(); i++) {
                vkDestroyDescriptorPool(device->getDevice(), descriptorPools[i], nullptr);
            }
            vkDestroyDescriptorSetLayout(device->getDevice(), layout, nullptr);
        }
        
        void VulkanDescriptorSetLayout::addField(VEngineDescriptorSetFieldType fieldType, VEngineDescriptorSetFieldStage fieldStage)
        {

            VkShaderStageFlags fieldStageInternal = VK_SHADER_STAGE_ALL;
            if (fieldStage == VEngineDescriptorSetFieldStage::FieldStageAll) fieldStageInternal = VK_SHADER_STAGE_ALL;
            if (fieldStage == VEngineDescriptorSetFieldStage::FieldStageAllGraphics) fieldStageInternal = VK_SHADER_STAGE_ALL_GRAPHICS;
            if (fieldStage == VEngineDescriptorSetFieldStage::FieldStageCompute) fieldStageInternal = VK_SHADER_STAGE_COMPUTE_BIT;
            if (fieldStage == VEngineDescriptorSetFieldStage::FieldStageVertex) fieldStageInternal = VK_SHADER_STAGE_VERTEX_BIT;
            if (fieldStage == VEngineDescriptorSetFieldStage::FieldStageFragment) fieldStageInternal = VK_SHADER_STAGE_FRAGMENT_BIT;

            VkDescriptorType descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            if (fieldType == VEngineDescriptorSetFieldType::FieldTypeSampler) descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            if (fieldType == VEngineDescriptorSetFieldType::FieldTypeUniformBuffer) descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            if (fieldType == VEngineDescriptorSetFieldType::FieldTypeStorageBuffer) descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            if (fieldType == VEngineDescriptorSetFieldType::FieldTypeStorageImage) descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;

            VkDescriptorSetLayoutBinding b = {};
            b.binding = static_cast<uint32_t>(bindings.size());
            b.descriptorType = descriptorType;
            b.descriptorCount = 1;
            b.stageFlags = fieldStageInternal;
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
            compiled = true;
        }

        DescriptorSetInterface* VulkanDescriptorSetLayout::generateDescriptorSet()
        {
            if (!compiled) {
                compile();
            }
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
            if (!compiled) {
                compile();
            }
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

    }
}