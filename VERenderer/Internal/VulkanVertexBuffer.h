#pragma once
class VulkanDevice;
#include <vulkan.h>
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
    void drawInstanced(VulkanGraphicsPipeline* p, std::vector<VulkanDescriptorSet*> sets, VulkanCommandBuffer* cb, size_t instances);
private:
    VulkanDevice * device;
    VkBuffer vertexBuffer;
    VulkanSingleAllocation vertexBufferMemory;
    int vertexCount = 0;
};

