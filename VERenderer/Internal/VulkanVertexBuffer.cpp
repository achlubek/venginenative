#include "stdafx.h"
#include "VulkanVertexBuffer.h"
#include "VulkanMemoryManager.h"
#include "VulkanCommandBuffer.h"
#include "VulkanGraphicsPipeline.h"
#include "../VulkanToolkit.h"
#include "../VulkanDescriptorSet.h"
#include <vulkan.h>


VulkanVertexBuffer::VulkanVertexBuffer(VulkanToolkit* vulkan, std::vector<float> data)
    : vulkan(vulkan)
{
    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = sizeof(data[0]) * data.size();
    bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(vulkan->device, &bufferInfo, nullptr, &vertexBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to create vertex buffer!");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(vulkan->device, vertexBuffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = vulkan->findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    auto properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

    vertexBufferMemory = vulkan->memoryManager->bindBufferMemory(vulkan->findMemoryType(memRequirements.memoryTypeBits, properties), vertexBuffer, memRequirements.size);

    void* ptr;
    vertexBufferMemory.map(&ptr);
    memcpy(ptr, data.data(), (size_t)bufferInfo.size);
    vertexBufferMemory.unmap();

    vertexCount = (int)(data.size() / 12);
}


VulkanVertexBuffer::~VulkanVertexBuffer()
{
    vertexBufferMemory.free();
    vkDestroyBuffer(vulkan->device, vertexBuffer, nullptr);
}

VkBuffer VulkanVertexBuffer::getBuffer()
{
    return vertexBuffer;
}

void VulkanVertexBuffer::drawInstanced(VulkanGraphicsPipeline * p, std::vector<VulkanDescriptorSet*> sets, VulkanCommandBuffer* cb, size_t instances)
{
    std::vector<VkDescriptorSet> realsets = {};
    for (int i = 0; i < sets.size(); i++) {
        realsets.push_back(sets[i]->set);
    }
    vkCmdBindDescriptorSets(cb->handle, VK_PIPELINE_BIND_POINT_GRAPHICS, p->pipelineLayout, 0, realsets.size(), realsets.data(), 0, nullptr);
    VkBuffer vertexBuffers[] = { vertexBuffer };
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(cb->handle, 0, 1, vertexBuffers, offsets);

    vkCmdDraw(cb->handle, static_cast<uint32_t>(vertexCount), instances, 0, 0);
}
