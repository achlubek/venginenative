#pragma once
#include "Enums.h"

namespace VEngine
{
    namespace Renderer
    {
        class ShaderModuleInterface
        {
        public:
            virtual VEngineShaderModuleType getType() = 0;
        };
    }
}