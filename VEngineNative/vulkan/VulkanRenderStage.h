#pragma once
class Object3dInfo;
class VulkanRenderStage
{
public:
	VulkanRenderStage();
	~VulkanRenderStage();

	VulkanRenderPass renderPass;
	VulkanFramebuffer framebuffer;
	VulkanGraphicsPipeline pipeline;
	std::vector<VulkanCommandBuffer> commandBuffers;
	std::vector<VkDescriptorSetLayout> setLayouts;
	std::vector<VulkanImage> outputImages;
	VkExtent2D viewport;
	std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
	
	void addDescriptorSetLayout(VkDescriptorSetLayout lay);
	void addOutputImage(VulkanImage lay);
	void setViewport(VkExtent2D size);
	void addShaderStage(VkPipelineShaderStageCreateInfo ss);

	int cmdBufferIndex = 0;
	void beginDrawing();
	void endDrawing();
	void drawMesh(Object3dInfo *info, size_t instances);
	void compile();

	void submit(std::vector<VkSemaphore> waitSemaphores, VkSemaphore &signalSemaphore);

};

