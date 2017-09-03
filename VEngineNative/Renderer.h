#pragma once 
class VulkanDescriptorSetLayout;
class VulkanDescriptorSet;
class Mesh3d;
class VulkanGenericBuffer;
class VulkanImage;
class VulkanRenderer;
class Camera;

class Renderer
{
public:
    Renderer(int iwidth, int iheight);
    ~Renderer(); 

    VulkanDescriptorSetLayout* meshSetLayout;
    VulkanDescriptorSet postProcessSet;
    VulkanDescriptorSet sharedSet;
    Mesh3d* dummyMesh;

    VulkanGenericBuffer uboHighFrequencyBuffer;
    VulkanGenericBuffer uboLowFrequencyBuffer;
    VulkanImage diffuseImage;
    VulkanImage normalImage;
    VulkanImage distanceImage;
    VulkanImage ambientImage;
    VulkanImage depthImage;

    VulkanRenderer* renderer;
     
    void renderToSwapChain(Camera *camera);
    int width;
    int height; 
};
