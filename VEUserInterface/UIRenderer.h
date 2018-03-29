#pragma once
class VulkanRenderer;
class VulkanToolkit;
class UIAbsDrawable;

class UIRenderer
{
public:
    UIRenderer(VulkanToolkit* vulkan, int width, int height);
    ~UIRenderer();
	void draw();
	void addDrawable(UIAbsDrawable* drawable);
	void removeDrawable(UIAbsDrawable* drawable);
	std::vector<UIAbsDrawable*> getDrawables();
	void removeAllDrawables();
	VulkanDescriptorSetLayout* layout{ nullptr };
	VulkanImage* dummyTexture;
	int width{ 0 }, height{ 0 };
	VulkanToolkit* vulkan{ nullptr };
	VulkanImage* outputImage;
private:
	VulkanRenderer * renderer{ nullptr };
    VulkanRenderStage* stage{ nullptr };
    std::vector<UIAbsDrawable*> drawables;
};

