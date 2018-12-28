#pragma once
#include <vector>
#include "Enums.h"

namespace VEngine
{
    namespace Renderer
    {
        class RenderStageInterface;
        class ShaderModuleInterface;
        class DescriptorSetLayoutInterface;
        class AttachmentInterface;

        class RenderStageFactoryInterface
        {
        public:
            virtual RenderStageInterface* build(int width, int height,
                std::vector<ShaderModuleInterface*> shaders,
                std::vector<DescriptorSetLayoutInterface*> layouts) = 0;

            virtual RenderStageInterface* build(int width, int height,
                std::vector<ShaderModuleInterface*> shaders,
                std::vector<DescriptorSetLayoutInterface*> layouts,
                std::vector<AttachmentInterface*> outputImages) = 0;

            virtual RenderStageInterface* build(int width, int height,
                std::vector<ShaderModuleInterface*> shaders,
                std::vector<DescriptorSetLayoutInterface*> layouts,
                VEngineCullMode cullMode) = 0;

            virtual RenderStageInterface* build(int width, int height,
                std::vector<ShaderModuleInterface*> shaders,
                std::vector<DescriptorSetLayoutInterface*> layouts,
                std::vector<AttachmentInterface*> outputImages,
                VEngineCullMode cullMode) = 0;
        };

    }
}