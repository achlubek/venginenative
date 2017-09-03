#pragma once
#include "Object3dInfo.h"
#include "Camera.h"
#include "FrustumCone.h"
#include "vulkan/VulkanRenderStage.h"
#include "VulkanRenderer.h"
#include "INIReader.h"
#include "Mesh3d.h"

class ShadowMapRenderer
{
public:
    ShadowMapRenderer(int width, int height);
    ~ShadowMapRenderer();
    VulkanDescriptorSetLayout* meshSetLayout;
    
    VulkanGenericBuffer uboHighFrequencyBuffer;
    VulkanGenericBuffer uboLowFrequencyBuffer;
    VulkanImage depthImage;

    VulkanRenderer* renderer;

    void renderToSwapChain(Camera *camera);
    int width;
    int height;
};

