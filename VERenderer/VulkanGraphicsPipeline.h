#pragma once
class VulkanToolkit;
class VulkanGraphicsPipeline
{
public:
    VulkanToolkit * vulkan;
    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline; 
    VulkanGraphicsPipeline(VulkanToolkit * vulkan, int viewportwidth, int viewportheight, std::vector<VkDescriptorSetLayout> setlayouts,
        std::vector<VkPipelineShaderStageCreateInfo> shaderstages, VulkanRenderPass* renderpass, bool enableDepthTest, bool alpha_blend);
    ~VulkanGraphicsPipeline();
};

