#pragma once

namespace VEngine
{
    namespace Renderer
    {
        class VulkanDescriptorSetLayout;
        namespace Internal
        {

            class VulkanDevice;
            class VulkanRenderPass;

            class VulkanGraphicsPipeline
            {
            public:
                VulkanGraphicsPipeline(VulkanDevice * device, int viewportwidth, int viewportheight, std::vector<VulkanDescriptorSetLayout*> setlayouts,
                    std::vector<VkPipelineShaderStageCreateInfo> shaderstages, VulkanRenderPass* renderpass, bool enableDepthTest,
                    VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VkCullModeFlags cullflags = 0); // todo omg this needs refactor

                VulkanGraphicsPipeline(VulkanDevice * device, std::vector<VulkanDescriptorSetLayout*> setlayouts,
                    VkPipelineShaderStageCreateInfo shader);

                ~VulkanGraphicsPipeline();

                VkPipeline getPipeline();
                VkPipelineLayout getPipelineLayout();

            private:
                VulkanDevice * device;
                VkPipelineLayout pipelineLayout;
                VkPipeline graphicsPipeline;
            };

        }
    }
}