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
            void addField(VEngineDescriptorSetFieldType fieldType, VEngineDescriptorSetFieldStage fieldAccesibility);
            DescriptorSetInterface* generateDescriptorSet();
        };

    }
}