#pragma once
class VulkanDevice;
class VulkanGraphicsPipeline;
class VulkanDescriptorSet;
class VulkanCommandBuffer;
#include "VulkanSingleAllocation.h"
class VulkanVertexBuffer
{
public:
    VulkanVertexBuffer(VulkanDevice* device, std::vector<float> data);
    ~VulkanVertexBuffer();
    VkBuffer getBuffer();
    void drawInstanced(VulkanGraphicsPipeline* p, std::vector<VulkanDescriptorSet*> sets, VulkanCommandBuffer* cb, uint32_t instances);
private:
    VulkanDevice * device;
    VkBuffer vertexBuffer;
    VulkanSingleAllocation vertexBufferMemory;
    int vertexCount = 0;
};

