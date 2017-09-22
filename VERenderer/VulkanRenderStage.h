#pragma once
class Object3dInfo;
class VulkanRenderStage;
class VulkanToolkit;
class VulkanRenderStage
{
public:
    VulkanRenderStage(VulkanToolkit * vulkan);
    ~VulkanRenderStage();

    VulkanToolkit * vulkan;
    VkSemaphore signalSemaphore;
    VulkanRenderPass* renderPass;
    VulkanFramebuffer* framebuffer;
    VulkanGraphicsPipeline* pipeline;
    VulkanCommandBuffer* commandBuffer;
    std::vector<VkDescriptorSetLayout> setLayouts;
    std::vector<VulkanImage*> outputImages;
    VkExtent2D viewport;
    std::vector<VkPipelineShaderStageCreateInfo> shaderStages;

    VulkanDescriptorSet* meshSharedSet;

    void addDescriptorSetLayout(VkDescriptorSetLayout lay);
    void addOutputImage(VulkanImage * lay);
    void setViewport(VkExtent2D size);
    void setViewport(int width, int height);
    void addShaderStage(VkPipelineShaderStageCreateInfo ss);
     
    int cmdMeshesCounts = 0;
    void beginDrawing();
    void endDrawing();
    void drawMesh(Object3dInfo *info, std::vector<VulkanDescriptorSet*> sets, size_t instances);
    void compile();
    VulkanRenderStage* copy();
    bool alphaBlending = false;
    bool additiveBlending = false;

    void submit(std::vector<VkSemaphore> waitSemaphores);
    void submitNoSemaphores(std::vector<VkSemaphore> waitSemaphores);

};

