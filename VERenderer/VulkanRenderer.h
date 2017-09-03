#pragma once
class VulkanRenderStage;
class VulkanRenderer
{
public:
    VulkanRenderer();
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
    VkSemaphore imageAvailableSemaphore;
    bool ppRecorded = false;
    Object3dInfo* postprocessmesh;
    VulkanRenderStage* meshStage;
    VulkanDescriptorSet* postProcessSet;
    std::vector<VulkanRenderStage*> postProcessingStages;
    std::vector<VulkanRenderStage*> outputStages;
    VulkanRenderStage* outputStageZygote;
};

