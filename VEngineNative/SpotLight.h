#pragma once
#include "AbsTransformable.h"
#include "TransformationManager.h"
class ShadowMapRenderer;
class VulkanToolkit;

class SpotLight : public AbsTransformable
{
public: 
    SpotLight(VulkanToolkit* vulkan, glm::vec3 color, TransformationManager *transmgr);
    ~SpotLight();
    unsigned int id;
    VulkanToolkit* vulkan;
    glm::vec3 color;
    void enableShadowMapping(int width, int height);
    void disableShadowMapping();
    bool isShadowMappingEnabled();
    void setFov(float fov);
    void update();
    void recordResolveCommands(VulkanRenderStage* stage, VulkanDescriptorSet* postProcessingSet);
private:
    VulkanDescriptorSet* lightDataSet;

    VulkanGenericBuffer* lightDataBuffer;
    ShadowMapRenderer* shadowMapRenderer;
    Camera* shadowMapCamera;
    bool shadowMappingEnabled = false;
    float fov;
};

