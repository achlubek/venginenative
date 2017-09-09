#pragma once  
class VulkanDescriptorSetLayout;
class VulkanGenericBuffer;
class VulkanImage;
class VulkanToolkit;
class Camera;

class ShadowMapRenderer
{
public:
    ShadowMapRenderer(VulkanToolkit * vulkan, int width, int height);
    ~ShadowMapRenderer();
    VulkanToolkit * vulkan;
    VulkanDescriptorSetLayout* meshSetLayout;
    
    VulkanGenericBuffer* uboHighFrequencyBuffer;
    VulkanGenericBuffer* uboLowFrequencyBuffer;
    VulkanImage* depthImage;

    VulkanRenderer* renderer;

    void renderToSwapChain(Camera *camera);
    int width;
    int height;
};

