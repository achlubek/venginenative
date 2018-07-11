#pragma once
class Object3dInfo;
class VulkanDevice;
class VulkanShaderModule;
class VulkanDescriptorSetLayout;
class VulkanImage;
class VulkanRenderPass;
class VulkanFramebuffer;
class VulkanGraphicsPipeline;
class VulkanCommandBuffer;

class VulkanRenderStage
{
public:
    VulkanRenderStage(VulkanDevice * device, int width, int height,
        std::vector<VulkanShaderModule*> shaders, 
        std::vector<VulkanDescriptorSetLayout*> layouts, 
        std::vector<VulkanImage*> outputImages);
    ~VulkanRenderStage();
private:

    VulkanDevice * device;
    VkSemaphore signalSemaphore;
    VulkanRenderPass* renderPass = nullptr;
    VulkanFramebuffer* framebuffer = nullptr;
    VulkanGraphicsPipeline* pipeline = nullptr;
    VulkanCommandBuffer* commandBuffer = nullptr;
    std::vector<VulkanDescriptorSetLayout*> setLayouts;
    std::vector<VulkanDescriptorSet*> sets;
    std::vector<VulkanImage*> outputImages;
    int width;
    int height;
    std::vector<VulkanShaderModule*> shaders;

    VulkanDescriptorSet* meshSharedSet;

    int cmdMeshesCounts = 0;
    void beginDrawing();
    void endDrawing();
    void setSets(std::vector<VulkanDescriptorSet*> sets);
    void drawMesh(Object3dInfo *info, size_t instances);
    void compile();
    VulkanRenderStage* copy();
    bool alphaBlending = false;
    bool additiveBlending = false;
    bool enabled = true;
    bool clearBeforeDrawing = true;
    VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST; 
    VkCullModeFlags cullFlags = 0;

    void submit(std::vector<VkSemaphore> waitSemaphores);
    void submitNoSemaphores(std::vector<VkSemaphore> waitSemaphores);

};

