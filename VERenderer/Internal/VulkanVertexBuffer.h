#pragma once
class VulkanToolkit;
class VkBuffer;
class VulkanGraphicsPipeline;
class VulkanDescriptorSet;
class VulkanCommandBuffer;
#include "VulkanSingleAllocation.h"
class VulkanVertexBuffer
{
public:
    VulkanVertexBuffer(VulkanToolkit* vulkan, std::vector<float> data);
    ~VulkanVertexBuffer();
    VkBuffer getBuffer();
    void drawInstanced(VulkanGraphicsPipeline* p, std::vector<VulkanDescriptorSet*> sets, VulkanCommandBuffer* cb, size_t instances);
private:
    VulkanToolkit * vulkan;
    VkBuffer vertexBuffer;
    VulkanSingleAllocation vertexBufferMemory;
    int vertexCount = 0;
};

