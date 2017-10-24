#pragma once
class Object3dInfo;
class VulkanToolkit;
class VulkanShaderModule;
class VulkanRenderStage
{
public:
    VulkanRenderStage(VulkanToolkit * vulkan);
    ~VulkanRenderStage();

    VulkanToolkit * vulkan;
    VkSemaphore signalSemaphore;
    VulkanRenderPass* renderPass = nullptr;
    VulkanFramebuffer* framebuffer = nullptr;
    VulkanGraphicsPipeline* pipeline = nullptr;
    VulkanCommandBuffer* commandBuffer = nullptr;
    std::vector<VkDescriptorSetLayout> setLayouts;
    std::vector<VulkanDescriptorSet*> sets;
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
    void setSets(std::vector<VulkanDescriptorSet*> sets);
    void drawMesh(Object3dInfo *info, size_t instances);
    void compile();
    VulkanRenderStage* copy();
    bool alphaBlending = false;
    bool additiveBlending = false;
    VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST; 
    VkCullModeFlags cullFlags = 0;

    void submit(std::vector<VkSemaphore> waitSemaphores);
    void submitNoSemaphores(std::vector<VkSemaphore> waitSemaphores);

};

