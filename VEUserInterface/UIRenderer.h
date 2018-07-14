#pragma once
class VulkanToolkit;
class UIAbsDrawable;
class Mouse;

class UIRenderer
{
public:
    UIRenderer(VulkanToolkit* vulkan, Mouse* mouse, VulkanImage* outputImage, int width, int height);
    ~UIRenderer();
	void draw();
	void addDrawable(UIAbsDrawable* drawable);
	void removeDrawable(UIAbsDrawable* drawable);
	std::vector<UIAbsDrawable*> getDrawables();
	void removeAllDrawables();
    std::vector<UIAbsDrawable*> rayCast(float x, float y);

    VulkanToolkit* getToolkit();
    uint32_t getWidth();
    uint32_t getHeight();
    VulkanDescriptorSetLayout* getSetLayout();
    VulkanImage* getDummyTexture();


private:
    VulkanRenderStage* stage{ nullptr };
    std::vector<UIAbsDrawable*> drawables;
    Mouse* mouse;
    VulkanToolkit* vulkan{ nullptr };
    uint32_t width{ 0 }, height{ 0 };
    VulkanDescriptorSetLayout* layout{ nullptr };
    VulkanImage* dummyTexture;
};

