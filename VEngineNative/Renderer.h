#pragma once 
class VulkanDescriptorSetLayout;
class VulkanDescriptorSet;
class Mesh3d;
class VulkanGenericBuffer;
class VulkanImage;
class VulkanRenderer;
class Camera;
class ShadowMapRenderer;
class VulkanToolkit;

class Renderer
{
public:
    Renderer(VulkanToolkit * vulkan, int iwidth, int iheight);
    ~Renderer(); 

    VulkanToolkit * vulkan;
    VulkanDescriptorSetLayout* meshSetLayout;
    VulkanDescriptorSet* postProcessSet;
    VulkanDescriptorSet* sharedSet;
    Mesh3d* dummyMesh;

    VulkanGenericBuffer* uboHighFrequencyBuffer;
    VulkanGenericBuffer* uboLowFrequencyBuffer;
    VulkanImage* diffuseImage;
    VulkanImage* normalImage;
    VulkanImage* distanceImage;
    VulkanImage* ambientImage;
    VulkanImage* depthImage;

    VulkanRenderer* renderer;

    ShadowMapRenderer* highResShadowMapper;
    Camera* shadowTestCamera;
     
    void renderToSwapChain(Camera *camera);
    int width;
    int height; 
};
