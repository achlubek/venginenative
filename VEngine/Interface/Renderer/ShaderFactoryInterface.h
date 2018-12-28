#pragma once
#include <string>
#include "Enums.h"

namespace VEngine
{
    namespace Renderer
    {
        class ShaderModuleInterface;

        class ShaderFactoryInterface
        {
        public:
            ShaderModuleInterface * build(VEngineShaderModuleType type, std::string mediakey);
        };

    }
}