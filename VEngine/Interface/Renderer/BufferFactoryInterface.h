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
            virtual GenericBufferInterface * build(VEngineBufferType type, uint64_t size) = 0;
        };

    }
}