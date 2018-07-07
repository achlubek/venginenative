#pragma once
class VulkanGraphicsPipeline;
class VulkanDescriptorSet;
#include "VulkanSingleAllocation.h"
class Object3dInfo
{
public:
    Object3dInfo(VulkanToolkit * vulkan, vector<float> &vbo);
    ~Object3dInfo();

    void draw(VulkanGraphicsPipeline* p, std::vector<VulkanDescriptorSet*> sets, VkCommandBuffer cb);
    void drawInstanced(VulkanGraphicsPipeline* p, std::vector<VulkanDescriptorSet*> sets, VkCommandBuffer cb, size_t instances);
    void updateRadius();
    float radius = 0.0f;
    vector<float> vbo;
    void rebufferVbo(vector<float> data, bool force_resize);

    VulkanToolkit * vulkan;

    VkBuffer vertexBuffer;
    VulkanSingleAllocation vertexBufferMemory;
    int vertexCount = 0;

    //VulkanImage texture;


private:

    void generate();

    bool generated = false;
};
