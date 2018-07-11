#pragma once
class VulkanDevice;
class VulkanRenderPass;
class VulkanDescriptorSetLayout;
#include <vulkan.h> 

class VulkanGraphicsPipeline
{
public:
    VulkanGraphicsPipeline(VulkanDevice * device, int viewportwidth, int viewportheight, std::vector<VulkanDescriptorSetLayout*> setlayouts,
        std::vector<VkPipelineShaderStageCreateInfo> shaderstages, VulkanRenderPass* renderpass, bool enableDepthTest, bool alpha_blend, 
        bool additive_blend, VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VkCullModeFlags cullflags = 0); // todo omg this needs refactor
    VulkanGraphicsPipeline(VulkanDevice * device, std::vector<VkDescriptorSetLayout> setlayouts,
        VkPipelineShaderStageCreateInfo shader);
    ~VulkanGraphicsPipeline();

    VkPipeline getPipeline();

private:
    VulkanDevice * device;
    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;
};

