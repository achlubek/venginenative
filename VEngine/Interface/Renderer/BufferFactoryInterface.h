#pragma once
#include "Enums.h"

namespace VEngine
{
    namespace Renderer
    {
        class GenericBufferInterface;

        class BufferFactoryInterface
        {
        public:
            GenericBufferInterface * build(VEngineBufferType type, uint64_t size);
        };

    }
}