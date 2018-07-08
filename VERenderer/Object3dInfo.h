#pragma once
class VulkanGraphicsPipeline;
class VulkanDescriptorSet;
class VulkanSingleAllocation;
class VulkanToolkit;
class VulkanCommandBuffer;
class VulkanVertexBuffer;
class Object3dInfo
{
public:
    Object3dInfo(VulkanToolkit * vulkan, std::vector<float> &vbo);
    ~Object3dInfo();

    VulkanVertexBuffer* getVertexBuffer();

private:
    VulkanToolkit * vulkan; 
    VulkanVertexBuffer* vertexBuffer;
};
