#pragma once

class VulkanRenderer;
class VulkanToolkit; 

struct PlanetInfo {
    glm::dvec3 position;
    double size;
    double noiseSeed;
    double starDistance;
};

struct StarInfo {
    glm::dvec3 position;
    double size;
    double noiseSeed;
};

class CosmosRenderer
{
public:
    CosmosRenderer(VulkanToolkit* ivulkan, int iwidth, int iheight);
    ~CosmosRenderer();
    /*
    the idea here is:
    choose max 4 closest planets
    choose max 2 closest stars
    choose closest object
    origin = closest object position
    render only those
    then render all stars with calculated brightness > of some threshold as points/sprites
    then render all stars with calculated brightness > of some threshold as points/sprites
    then use a 3d texture with density of the galaxy,m possible posisble with super cheap 2d texture or even fucking noise
    to render the "flux"
    thats it galaxy renderer

    what would be the steps:
    1 texture rgba 8bit for Star output, alpha is needed here
    1 texture rgba for Planet output alpha too important
    1 texture rgba for output rendering possibly doing the flux rendering etc

    then 3 stages
    stars raytracing
    planet raytracing
    combining stage

    stars input:
    NO NEED FOR STAR POSITION because its always 0.0
    all the camera data to be able to reconstruct rays
    float Size of star
    float Seed of star
    maybe texture sample noise simplex to simlify shit? may  be not needed at first
    probably thats it :d

    planets input:
    3 x struct{
    vec4 position
    vec4 size_seed_distance
    }

    */
    std::vector<PlanetInfo> planets;
    std::vector<StarInfo> stars;
    std::vector<PlanetInfo> closestPlanets;
    StarInfo closestStar;

    int width{ 0 };
    int height{ 0 };

    VulkanToolkit* vulkan{ nullptr };
    VulkanRenderer* renderer{ nullptr };

    VulkanRenderStage* starsStage{ nullptr };
    VulkanDescriptorSetLayout* starsLayout{ nullptr };
    VulkanDescriptorSet* starsSet{ nullptr };

    VulkanRenderStage* planetsStage{ nullptr };
    VulkanDescriptorSetLayout* planetsLayout{ nullptr };
    VulkanDescriptorSet* planetsSet{ nullptr };

    VulkanRenderStage* combineStage{ nullptr };
    VulkanDescriptorSetLayout* combineLayout{ nullptr };
    VulkanDescriptorSet* combineSet{ nullptr };

    VulkanGenericBuffer* cameraDataBuffer;
    VulkanGenericBuffer* closestStarData;
    VulkanGenericBuffer* closestPlanetsData;

    VulkanImage* starsImage;
    VulkanImage* planetsImage;
    VulkanImage* outputImage;

    void updateClosestPlanetsAndStar(Camera * camera);

    void updateCameraBuffer(Camera* cam);
    void updateObjectsBuffers(Camera * camera);
    void draw();
};

