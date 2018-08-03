#pragma once

namespace VEngine
{
    namespace Renderer
    {
        namespace Internal
        {
            class VulkanDevice;
            class VulkanRenderPass;
            class VulkanFramebuffer;
            class VulkanGraphicsPipeline;
            class VulkanCommandBuffer;
        }
        class Object3dInfo;
        class VulkanShaderModule;
        class VulkanDescriptorSet;
        class VulkanDescriptorSetLayout;
        class VulkanAttachment;

        class VulkanRenderStage
        {
        public:
            VulkanRenderStage(Internal::VulkanDevice * device, int width, int height,
                std::vector<VulkanShaderModule*> shaders,
                std::vector<VulkanDescriptorSetLayout*> layouts,
                std::vector<VulkanAttachment*> outputImages);
            ~VulkanRenderStage();
            void beginDrawing();
            void endDrawing();
            void setSets(std::vector<VulkanDescriptorSet*> sets);
            void drawMesh(Object3dInfo *info, uint32_t instances);
            void submit(std::vector<VkSemaphore> waitSemaphores);
            void submitNoSemaphores(std::vector<VkSemaphore> waitSemaphores);
            VulkanRenderStage* copy();
            VulkanRenderStage* copy(std::vector<VulkanAttachment*> outputImages);
            VkSemaphore getSignalSemaphore();

        private:
            Internal::VulkanDevice * device;
            VkSemaphore signalSemaphore;
            Internal::VulkanRenderPass* renderPass = nullptr;
            Internal::VulkanFramebuffer* framebuffer = nullptr;
            Internal::VulkanGraphicsPipeline* pipeline = nullptr;
            Internal::VulkanCommandBuffer* commandBuffer = nullptr;
            std::vector<VulkanDescriptorSetLayout*> setLayouts;
            std::vector<VulkanDescriptorSet*> sets;
            std::vector<VulkanAttachment*> outputImages;
            int width;
            int height;
            std::vector<VulkanShaderModule*> shaders;

            VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            VkCullModeFlags cullFlags = VK_CULL_MODE_NONE;
            bool compiled = false;
            void compile();
        };

    }
}
