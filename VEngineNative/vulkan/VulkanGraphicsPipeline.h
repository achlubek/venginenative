#pragma once
class VulkanGraphicsPipeline
{
public:
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;
	VulkanGraphicsPipeline() {}
	VulkanGraphicsPipeline(int viewportwidth, int viewportheight, std::vector<VkDescriptorSetLayout> setlayouts,
		std::vector<VkPipelineShaderStageCreateInfo> shaderstages, VulkanRenderPass renderpass, bool enableDepthTest);
	~VulkanGraphicsPipeline();
};

