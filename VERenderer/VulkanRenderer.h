#pragma once
class VulkanRenderStage;
class VulkanToolkit;
class VulkanRenderer
{
public:
    VulkanToolkit * vulkan;
    VulkanRenderer(VulkanToolkit * vulkan);
    ~VulkanRenderer();

    void setMeshStage(VulkanRenderStage* stage);
    void setOutputStage(VulkanRenderStage* stage);
    void addPostProcessingStage(VulkanRenderStage* stage);
    void setPostProcessingDescriptorSet(VulkanDescriptorSet* set);
    VulkanRenderStage* getMesh3dStage();
    void compile();
    void beginDrawing();
    void endDrawing();

private:
    void submitEmptyBatch(std::vector<VkSemaphore> waitSemaphores, VkSemaphore signalSemaphore);
    void submitEmptyBatch2(std::vector<VkSemaphore> waitSemaphores);
    void submitEmptyBatch3(VkSemaphore signalSemaphore);
    VkSemaphore imageAvailableSemaphore;
    VkSemaphore stubMeshSemaphore;
    bool ppRecorded = false;
    bool usingMeshStage = false;
    Object3dInfo* postprocessmesh;
    VulkanRenderStage* meshStage = nullptr;
    VulkanDescriptorSet* postProcessSet = nullptr;
    std::vector<VulkanRenderStage*> postProcessingStages;
    std::vector<VulkanRenderStage*> outputStages;
    VulkanRenderStage* outputStageZygote = nullptr;
};

