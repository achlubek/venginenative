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
	VulkanCommandBuffer commandBuffer;
	std::vector<VkDescriptorSetLayout> setLayouts;
	std::vector<VulkanImage> outputImages;
	VkExtent2D viewport;
	std::vector<VkPipelineShaderStageCreateInfo> shaderStages;

	std::vector<Object3dInfo> meshes;

	void addDescriptorSetLayout(VkDescriptorSetLayout lay);
	void addOutputImage(VulkanImage lay);
	void setViewport(VkExtent2D size);
	void addShaderStage(VkPipelineShaderStageCreateInfo ss);

	void updateCommandBuffer();
	void compile();

	void submit(std::vector<VkSemaphore> waitSemaphores, VkSemaphore &signalSemaphore);

};

