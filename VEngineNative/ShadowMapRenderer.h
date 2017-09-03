#pragma once  
class VulkanDescriptorSetLayout;
class VulkanGenericBuffer;
class VulkanImage;
class Camera;

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

