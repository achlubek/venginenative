#pragma once
#include "../Interface/Renderer/Enums.h"
#include "../Interface/Renderer/RenderStageInterface.h"

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
        class SemaphoreFactoryInterface;

        class VulkanRenderStage : public RenderStageInterface
        {
        public:
            VulkanRenderStage(Internal::VulkanDevice * device, SemaphoreFactoryInterface* semaphoreFactory, int width, int height,
                std::vector<ShaderModuleInterface*> shaders,
                std::vector<DescriptorSetLayoutInterface*> layouts,
                std::vector<AttachmentInterface*> outputImages,
                VEngineCullMode cullMode);
            ~VulkanRenderStage();
            virtual void beginDrawing() override;
            virtual void endDrawing() override;
            virtual void setSets(std::vector<DescriptorSetInterface*> sets) override;
            virtual void setSet(size_t index, DescriptorSetInterface* set) override;
            virtual void drawMesh(Object3dInfoInterface *info, uint32_t instances) override;
            virtual void submit(std::vector<SemaphoreInterface*> waitSemaphores) override;
            virtual void submitNoSemaphores(std::vector<SemaphoreInterface*> waitSemaphores) override;
            virtual VulkanRenderStage* copy() override;
            virtual VulkanRenderStage* copy(std::vector<AttachmentInterface*> outputImages) override;
            virtual SemaphoreInterface* getSignalSemaphore() override;

        private:
            Internal::VulkanDevice * device;
            SemaphoreInterface* signalSemaphore;
            SemaphoreFactoryInterface* semaphoreFactory;
            Internal::VulkanRenderPass* renderPass = nullptr;
            Internal::VulkanFramebuffer* framebuffer = nullptr;
            Internal::VulkanGraphicsPipeline* pipeline = nullptr;
            Internal::VulkanCommandBuffer* commandBuffer = nullptr;
            std::vector<DescriptorSetLayoutInterface*> setLayouts;
            std::vector<DescriptorSetInterface*> sets;
            std::vector<AttachmentInterface*> outputImages;
            int width;
            int height;
            std::vector<ShaderModuleInterface*> shaders;

            VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            VkCullModeFlags cullFlags = VK_CULL_MODE_NONE;
            VEngineCullMode cullMode;
            bool compiled = false;
            void compile();
        };

    }
}
