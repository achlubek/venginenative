#include "stdafx.h"
#include "Renderer.h"
#include "Game.h"
#include "Camera.h"
#include "FrustumCone.h"
#include "Media.h"

Renderer::Renderer(int iwidth, int iheight)
{
    width = iwidth;
    height = iheight;
	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	if (vkCreateSemaphore(VulkanToolkit::singleton->device, &semaphoreInfo, nullptr, &imageAvailableSemaphore) != VK_SUCCESS ||
		vkCreateSemaphore(VulkanToolkit::singleton->device, &semaphoreInfo, nullptr, &renderFinishedSemaphore1) != VK_SUCCESS ||
		vkCreateSemaphore(VulkanToolkit::singleton->device, &semaphoreInfo, nullptr, &renderFinishedSemaphore2) != VK_SUCCESS) {
		throw std::runtime_error("failed to create semaphores!");
	}

	diffuseImage = VulkanImage(width, height, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_PREINITIALIZED, false);


	depthImage = VulkanImage(width, height, VK_FORMAT_D32_SFLOAT, VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_DEPTH_BIT, VK_IMAGE_LAYOUT_PREINITIALIZED, true);

	uboHighFrequencyBuffer = VulkanGenericBuffer(sizeof(float) * 4);
	uboLowFrequencyBuffer = VulkanGenericBuffer(sizeof(float) * 4);

	//setManager = VulkanDescriptorSetsManager();

	auto vertShaderModule = VulkanShaderModule("../../shaders/compiled/triangle.vert.spv");
	auto fragShaderModule = VulkanShaderModule("../../shaders/compiled/triangle.frag.spv");

	meshRenderStage = VulkanRenderStage();
	VkExtent2D ext = VkExtent2D();
	ext.width = width;
	ext.height = height;
	meshRenderStage.setViewport(ext);
	meshRenderStage.addShaderStage(vertShaderModule.createShaderStage(VK_SHADER_STAGE_VERTEX_BIT, "main"));
	meshRenderStage.addShaderStage(fragShaderModule.createShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT, "main"));
	meshRenderStage.addDescriptorSetLayout(setManager.mesh3dLayout);
	meshRenderStage.addOutputImage(diffuseImage);
	meshRenderStage.addOutputImage(depthImage);

	meshRenderStage.compile();


	auto ppvertShaderModule = VulkanShaderModule("../../shaders/compiled/pp.vert.spv");
	auto ppfragShaderModule = VulkanShaderModule("../../shaders/compiled/pp.frag.spv");
	postProcessRenderStages.resize(VulkanToolkit::singleton->swapChain->swapChainImages.size());
	for (int i = 0; i < VulkanToolkit::singleton->swapChain->swapChainImages.size(); i++) {

		postProcessRenderStages[i] = VulkanRenderStage();

		postProcessRenderStages[i].setViewport(ext);
		postProcessRenderStages[i].addShaderStage(ppvertShaderModule.createShaderStage(VK_SHADER_STAGE_VERTEX_BIT, "main"));
		postProcessRenderStages[i].addShaderStage(ppfragShaderModule.createShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT, "main"));
		postProcessRenderStages[i].addDescriptorSetLayout(setManager.ppLayout);
		VkFormat format = VulkanToolkit::singleton->swapChain->swapChainImageFormat;
		VulkanImage* img = new VulkanImage(format, VulkanToolkit::singleton->swapChain->swapChainImages[i], VulkanToolkit::singleton->swapChain->swapChainImageViews[i]);
		img->format = format;
		img->image = VulkanToolkit::singleton->swapChain->swapChainImages[i];
		img->imageView = VulkanToolkit::singleton->swapChain->swapChainImageViews[i];
		img->isPresentReady = true;
		postProcessRenderStages[i].addOutputImage(*img);

		postProcessRenderStages[i].compile();
	}

	postprocessmesh = Game::instance->asset->loadObject3dInfoFile("ppplane.vbo");
	postProcessSet = setManager.generatePostProcessingDescriptorSet();
	postProcessSet.bindUniformBuffer(0, uboHighFrequencyBuffer);
	postProcessSet.bindUniformBuffer(1, uboLowFrequencyBuffer);
	postProcessSet.bindImageViewSampler(2, diffuseImage);
	postProcessSet.update();

	//postprocessmesh->descriptorSet = meshSetManager.generateMesh3dDescriptorSet();
	//postprocessmesh->descriptorSet.bindUniformBuffer(0, uniformBuffer);
	//postprocessmesh->descriptorSet.bindImageViewSampler(1, colorImage);
	//postprocessmesh->descriptorSet.update();
}

Renderer::~Renderer()
{
	vkDestroySemaphore(VulkanToolkit::singleton->device, renderFinishedSemaphore1, nullptr);
	vkDestroySemaphore(VulkanToolkit::singleton->device, renderFinishedSemaphore2, nullptr);
	vkDestroySemaphore(VulkanToolkit::singleton->device, imageAvailableSemaphore, nullptr);
}

void Renderer::renderToSwapChain(Camera *camera)
{

	if (Game::instance->world->scene->getMesh3ds().size() == 0) return;
	VulkanBinaryBufferBuilder bb = VulkanBinaryBufferBuilder();
	double xpos, ypos;
	glfwGetCursorPos(VulkanToolkit::singleton->window, &xpos, &ypos);

	bb.emplaceFloat32((float)glfwGetTime());
	bb.emplaceFloat32(0.0f);
	bb.emplaceFloat32((float)xpos / (float)width);
	bb.emplaceFloat32((float)ypos / (float)height);

	void* data;
	uboHighFrequencyBuffer.map(0, bb.buffer.size(), &data);
	memcpy(data, bb.getPointer(), bb.buffer.size());
	uboHighFrequencyBuffer.unmap();

	uboLowFrequencyBuffer.map(0, bb.buffer.size(), &data);
	memcpy(data, bb.getPointer(), bb.buffer.size());
	uboLowFrequencyBuffer.unmap();

	uint32_t imageIndex;
	// GET IMAGE 
	Game::instance->world->setUniforms(camera);
	Game::instance->world->setSceneUniforms();
	meshRenderStage.beginDrawing();
	Game::instance->world->draw(&meshRenderStage, camera);
	meshRenderStage.endDrawing();
	if (meshRenderStage.cmdMeshesCounts == 0) return;
	vkQueueWaitIdle(VulkanToolkit::singleton->mainQueue);

	vkAcquireNextImageKHR(VulkanToolkit::singleton->device, VulkanToolkit::singleton->swapChain->swapChain, std::numeric_limits<uint64_t>::max(), imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

	vkQueueWaitIdle(VulkanToolkit::singleton->mainQueue);
	meshRenderStage.submit({ imageAvailableSemaphore }, renderFinishedSemaphore1);
	vkQueueWaitIdle(VulkanToolkit::singleton->mainQueue);
	//###########
	//if (!ppRecorded) {

		//for (int i = 0; i < VulkanToolkit::singleton->swapChain->swapChainImages.size(); i++) {
			postProcessRenderStages[imageIndex].beginDrawing();
			postProcessRenderStages[imageIndex].drawMesh(postprocessmesh, postProcessSet, 1);
			postProcessRenderStages[imageIndex].endDrawing();
		//}
		vkQueueWaitIdle(VulkanToolkit::singleton->mainQueue);
		ppRecorded = true;
//	}

	vkQueueWaitIdle(VulkanToolkit::singleton->mainQueue);
	postProcessRenderStages[imageIndex].submit({ renderFinishedSemaphore1 }, renderFinishedSemaphore2);

	vkQueueWaitIdle(VulkanToolkit::singleton->mainQueue);
	VulkanToolkit::singleton->swapChain->present({ renderFinishedSemaphore2 }, imageIndex);

	vkQueueWaitIdle(VulkanToolkit::singleton->mainQueue);
}
