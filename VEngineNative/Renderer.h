#pragma once
#include "Object3dInfo.h"
#include "Camera.h"
#include "FrustumCone.h"
#include "vulkan/VulkanRenderStage.h"
#include "VulkanRenderer.h"
#include "INIReader.h"
#include "Mesh3d.h"

class Renderer
{
public:
    Renderer(int iwidth, int iheight);
    ~Renderer(); 

    VulkanDescriptorSetsManager setManager;
    VulkanDescriptorSet postProcessSet;
    Mesh3d* dummyMesh;

    VulkanGenericBuffer uboHighFrequencyBuffer;
    VulkanGenericBuffer uboLowFrequencyBuffer;
    VulkanImage diffuseImage;
    VulkanImage normalImage;
    VulkanImage distanceImage;
    VulkanImage depthImage;

    VulkanRenderer* renderer;
     
    void renderToSwapChain(Camera *camera);
    int width;
    int height; 
};
