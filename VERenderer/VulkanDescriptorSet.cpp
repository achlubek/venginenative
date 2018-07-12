#include "stdafx.h"
#include "VulkanDescriptorSet.h"
#include "VulkanImage.h"
#include "VulkanGenericBuffer.h"
#include "Internal/VulkanDevice.h"

VulkanDescriptorSet::VulkanDescriptorSet(VulkanDevice * device, VkDescriptorPool p, VkDescriptorSetLayout l)
    : device(device)
{
    writes = {};
    pool = p;
    //layout = l;

    VkDescriptorSetLayout layouts[] = { l };
    VkDescriptorSetAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = pool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = layouts;
    vkAllocateDescriptorSets(device->getDevice(), &allocInfo, &set);
}

VulkanDescriptorSet::~VulkanDescriptorSet()
{
}

void VulkanDescriptorSet::bindImageViewSampler(int binding, VulkanImage* img)
{
    VkDescriptorImageInfo* imageInfo = new VkDescriptorImageInfo();
    imageInfo->imageLayout = VK_IMAGE_LAYOUT_GENERAL;
    imageInfo->imageView = img->getImageView();
    imageInfo->sampler = img->getSampler();

    writes.resize(writes.size() + 1);

    int i = writes.size() - 1;

    writes[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[i].dstSet = set;
    writes[i].dstBinding = binding;
    writes[i].dstArrayElement = 0;
    writes[i].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    writes[i].descriptorCount = 1;
    writes[i].pImageInfo = imageInfo;

}
void VulkanDescriptorSet::bindImageStorage(int binding, VulkanImage* img)
{
    VkDescriptorImageInfo* imageInfo = new VkDescriptorImageInfo();
    imageInfo->imageLayout = VK_IMAGE_LAYOUT_GENERAL;
    imageInfo->imageView = img->getImageView();
    imageInfo->sampler = img->getSampler();

    writes.resize(writes.size() + 1);

    int i = writes.size() - 1;

    writes[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[i].dstSet = set;
    writes[i].dstBinding = binding;
    writes[i].dstArrayElement = 0;
    writes[i].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    writes[i].descriptorCount = 1;
    writes[i].pImageInfo = imageInfo;

}

void VulkanDescriptorSet::bindUniformBuffer(int binding, VulkanGenericBuffer* buffer)
{
    VkDescriptorBufferInfo* bufferInfo = new VkDescriptorBufferInfo();
    bufferInfo->buffer = buffer->getBuffer();
    bufferInfo->offset = 0;
    bufferInfo->range = buffer->getSize();

    writes.resize(writes.size() + 1);

    int i = writes.size() - 1;

    writes[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[i].dstSet = set;
    writes[i].dstBinding = binding;
    writes[i].dstArrayElement = 0;
    writes[i].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    writes[i].descriptorCount = 1;
    writes[i].pBufferInfo = bufferInfo;
}

void VulkanDescriptorSet::bindStorageBuffer(int binding, VulkanGenericBuffer* buffer)
{
    VkDescriptorBufferInfo* bufferInfo = new VkDescriptorBufferInfo();
    bufferInfo->buffer = buffer->getBuffer();
    bufferInfo->offset = 0;
    bufferInfo->range = buffer->getSize();

    writes.resize(writes.size() + 1);

    int i = writes.size() - 1;

    writes[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[i].dstSet = set;
    writes[i].dstBinding = binding;
    writes[i].dstArrayElement = 0;
    writes[i].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    writes[i].descriptorCount = 1;
    writes[i].pBufferInfo = bufferInfo;
}

void VulkanDescriptorSet::update()
{
    vkUpdateDescriptorSets(device->getDevice(), static_cast<uint32_t>(writes.size()), writes.data(), 0, nullptr);
    writes.clear();
    writes = {};
}

VkDescriptorSet VulkanDescriptorSet::getSet()
{
    return set;
}
