#pragma once
#include "Object3dInfo.h"
#include "Camera.h"
#include "FrustumCone.h"
#include "vulkan/VulkanRenderStage.h"
#include "INIReader.h"

class Renderer
{
public:
	Renderer(int iwidth, int iheight);
	~Renderer();
	VkSemaphore imageAvailableSemaphore;
	VkSemaphore renderFinishedSemaphore1;
	VkSemaphore renderFinishedSemaphore2;
	VulkanRenderStage meshRenderStage;
	std::vector<VulkanRenderStage> postProcessRenderStages;

	VulkanDescriptorSetsManager meshSetManager;
	VulkanDescriptorSetsManager postProcessSetManager;

	VulkanGenericBuffer uniformBuffer;
	VulkanImage colorImage;
	VulkanImage depthImage;

	Object3dInfo* postprocessmesh;
	void renderToSwapChain(Camera *camera);
	int width;
	int height;
	bool ppRecorded = false;
};
