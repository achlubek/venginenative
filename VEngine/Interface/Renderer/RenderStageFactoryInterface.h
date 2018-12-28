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
            RenderStageInterface* build(int width, int height,
                std::vector<ShaderModuleInterface*> shaders,
                std::vector<DescriptorSetLayoutInterface*> layouts);

            RenderStageInterface* build(int width, int height,
                std::vector<ShaderModuleInterface*> shaders,
                std::vector<DescriptorSetLayoutInterface*> layouts,
                std::vector<AttachmentInterface*> outputImages);

            RenderStageInterface* build(int width, int height,
                std::vector<ShaderModuleInterface*> shaders,
                std::vector<DescriptorSetLayoutInterface*> layouts,
                VEngineCullMode cullMode);

            RenderStageInterface* build(int width, int height,
                std::vector<ShaderModuleInterface*> shaders,
                std::vector<DescriptorSetLayoutInterface*> layouts,
                std::vector<AttachmentInterface*> outputImages,
                VEngineCullMode cullMode);
        };

    }
}