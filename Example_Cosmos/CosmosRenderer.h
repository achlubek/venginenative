#pragma once

class VulkanRenderer;
class VulkanToolkit;
#include "GalaxyGenerator.h"

class CosmosRenderer
{
public:
    CosmosRenderer(VulkanToolkit* ivulkan, int iwidth, int iheight);
    ~CosmosRenderer();
    void recompileShaders(bool deleteOld);

    GalaxyGenerator* galaxy;
    UIRenderer* ui;
    AssetLoader assets;

    std::vector<GeneratedStarSystemInfo> nearbyStars;
    std::vector<GeneratedStarSystemInfo> nearestStarSystems;
    //int nearestStarIndex; 
    GeneratedPlanetInfo nearestPlanet;
    GeneratedMoonInfo nearestMoon;
    Camera* internalCamera;

    GeneratedStarInfo getStar(int starIndex);
    GeneratedPlanetInfo getPlanet(int starIndex, int planetIndex);
    GeneratedMoonInfo getMoon(int starIndex, int planetIndex, int moonIndex);

    int lastStarId;
    int lastPlanetId;

    int width{ 0 };
    int height{ 0 };

    VulkanToolkit* vulkan{ nullptr };
    VulkanRenderer* renderer{ nullptr };

    VulkanRenderStage* celestialStage{ nullptr };
    VulkanDescriptorSetLayout* celestialLayout{ nullptr };
    VulkanDescriptorSet* celestialSet{ nullptr };

    VulkanRenderStage* starsStage{ nullptr };
   // VulkanDescriptorSetLayout* starsLayout{ nullptr };
   // VulkanDescriptorSet* starsSet{ nullptr };

    VulkanRenderStage* planetDataStage{ nullptr };
    VulkanRenderStage* shipStage{ nullptr };

    VulkanRenderStage* combineStage{ nullptr };
    VulkanDescriptorSetLayout* combineLayout{ nullptr };
    VulkanDescriptorSet* combineSet{ nullptr };

    VulkanGenericBuffer* cameraDataBuffer;
    VulkanGenericBuffer* starsDataBuffer;
    VulkanGenericBuffer* planetsDataBuffer;
    VulkanGenericBuffer* moonsDataBuffer;
    VulkanGenericBuffer* shipDataBuffer;

    VulkanImage* celestialImage;
    VulkanImage* starsImage;
    //VulkanImage* cosmosImage;
    VulkanImage* shipResultImage;
    VulkanImage* shipDepthImage;
    VulkanImage* shipColorTexture;

    VulkanImage* planetTerrainHeightImage;
    VulkanImage* planetTerrainColorImage;
    VulkanImage* planetAtmosphereFlunctuationsImage;

    Object3dInfo* cube3dInfo;
    Object3dInfo* spaceship3dInfo;

    const double scale = 0.01;

    glm::vec3 lastGravity;
    glm::dvec3 closestBodyPosition;
    glm::dvec3 closestSurfacePosition;
    glm::dvec3 closestSurfaceNormal;
    glm::dvec3 closestObjectLinearAbsoluteSpeed;
    double closestSurfaceDistance;

    void* starsDataBufferPointer;
    void* planetsDataBufferPointer;
    void* moonsDataBufferPointer;

    void mapBuffers();
    void unmapBuffers();

    void updateStars();

    void updatePlanetsAndMoon(glm::dvec3 observerPosition);

    void updateGravity(glm::dvec3 observerPosition);

    void updateNearestStar(glm::dvec3 observerPosition);

    void updateCameraBuffer(Camera* cam, glm::dvec3 observerPosition, glm::dvec3 spaceShipPosition, glm::dquat spaceShipOrientation);
    void draw();

    volatile bool readyForDrawing = false;
};

