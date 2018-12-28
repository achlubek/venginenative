#pragma once
#include "Enums.h" 

namespace VEngine
{
    namespace Renderer
    {
        class DescriptorSetInterface;

        class DescriptorSetLayoutInterface
        {
        public:
            virtual void addField(VEngineDescriptorSetFieldType fieldType, VEngineDescriptorSetFieldStage fieldAccesibility) = 0;
            virtual DescriptorSetInterface* generateDescriptorSet() = 0;
        };

    }
}