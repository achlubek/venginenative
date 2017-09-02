#pragma once
class Object3dInfo;
class VulkanRenderStage;
class VulkanRenderStage
{
public:
	VulkanRenderStage();
	~VulkanRenderStage();

	VkSemaphore signalSemaphore;
	VulkanRenderPass renderPass;
	VulkanFramebuffer framebuffer;
	VulkanGraphicsPipeline pipeline;
	VulkanCommandBuffer commandBuffer;
	std::vector<VkDescriptorSetLayout> setLayouts;
	std::vector<VulkanImage> outputImages;
	VkExtent2D viewport;
	std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
	
	void addDescriptorSetLayout(VkDescriptorSetLayout lay);
	void addOutputImage(VulkanImage lay);
	void setViewport(VkExtent2D size);
	void addShaderStage(VkPipelineShaderStageCreateInfo ss);
	 
	int cmdMeshesCounts = 0;
	void beginDrawing();
	void endDrawing();
	void drawMesh(Object3dInfo *info, VulkanDescriptorSet &set, size_t instances);
	void compile();
	VulkanRenderStage* copy();

	void submit(std::vector<VkSemaphore> waitSemaphores);

};

