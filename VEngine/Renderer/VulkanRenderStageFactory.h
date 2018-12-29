#pragma once
#include "../Interface/Renderer/Enums.h"
#include "../Interface/Renderer/RenderStageFactoryInterface.h"

namespace VEngine
{
    namespace Renderer
    {
        namespace Internal
        {
            class VulkanDevice;
        }
        class SemaphoreFactoryInterface;

        class VulkanRenderStageFactory : public RenderStageFactoryInterface
        {
        public:
            VulkanRenderStageFactory(Internal::VulkanDevice* device, SemaphoreFactoryInterface* semaphoreFactory);
            ~VulkanRenderStageFactory();

            virtual RenderStageInterface* build(int width, int height,
                std::vector<ShaderModuleInterface*> shaders,
                std::vector<DescriptorSetLayoutInterface*> layouts) override;

            virtual RenderStageInterface* build(int width, int height,
                std::vector<ShaderModuleInterface*> shaders,
                std::vector<DescriptorSetLayoutInterface*> layouts,
                std::vector<AttachmentInterface*> outputImages) override;

            virtual RenderStageInterface* build(int width, int height,
                std::vector<ShaderModuleInterface*> shaders,
                std::vector<DescriptorSetLayoutInterface*> layouts,
                VEngineCullMode cullMode) override;

            virtual RenderStageInterface* build(int width, int height,
                std::vector<ShaderModuleInterface*> shaders,
                std::vector<DescriptorSetLayoutInterface*> layouts,
                std::vector<AttachmentInterface*> outputImages,
                VEngineCullMode cullMode) override;

        private:
            Internal::VulkanDevice * device;
            SemaphoreFactoryInterface* semaphoreFactory;
        };

    }
}