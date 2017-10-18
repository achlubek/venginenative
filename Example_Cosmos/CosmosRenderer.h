#pragma once

class VulkanRenderer;
class VulkanToolkit;
#include "GalaxyGenerator.h"

class CosmosRenderer
{
public:
    CosmosRenderer(VulkanToolkit* ivulkan, int iwidth, int iheight);
    ~CosmosRenderer();

    GalaxyGenerator* galaxy;

    std::vector<GeneratedStarInfo> nearbyStars;

    int width{ 0 };
    int height{ 0 };

    VulkanToolkit* vulkan{ nullptr };
    VulkanRenderer* renderer{ nullptr };

    VulkanRenderStage* celestialStage{ nullptr };
    VulkanDescriptorSetLayout* celestialLayout{ nullptr };
    VulkanDescriptorSet* celestialSet{ nullptr };

    VulkanRenderStage* combineStage{ nullptr };
    VulkanDescriptorSetLayout* combineLayout{ nullptr };
    VulkanDescriptorSet* combineSet{ nullptr };

    VulkanGenericBuffer* cameraDataBuffer;
    VulkanGenericBuffer* starsDataBuffer;
    VulkanGenericBuffer* planetsDataBuffer;
    VulkanGenericBuffer* moonsDataBuffer;

    VulkanImage* celestialImage;
    VulkanImage* outputImage;

    void updateClosestPlanetsAndStar(Camera * camera);

    void updateCameraBuffer(Camera* cam);
    void updateObjectsBuffers(Camera * camera);
    void draw();
};

