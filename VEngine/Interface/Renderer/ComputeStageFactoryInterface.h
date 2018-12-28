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
            virtual ComputeStageInterface * build(ShaderModuleInterface* shader, std::vector<DescriptorSetLayoutInterface*> layouts) = 0;
        };

    }
}