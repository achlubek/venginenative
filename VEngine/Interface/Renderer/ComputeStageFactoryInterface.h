#pragma once
#include <vector>

namespace VEngine
{
    namespace Renderer
    {
        class ComputeStageInterface;
        class ShaderModuleInterface;
        class DescriptorSetLayoutInterface;

        class ComputeStageFactoryInterface
        {
        public:
            ComputeStageInterface * build(ShaderModuleInterface* shader, std::vector<DescriptorSetLayoutInterface*> layouts);
        };

    }
}