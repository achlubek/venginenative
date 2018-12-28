#pragma once 
#include <cstdint>
#include "Enums.h"

namespace VEngine
{
    namespace Renderer
    {
        class GenericBufferInterface
        {
        public:
            void map(uint64_t offset, uint64_t size, void** data);
            void unmap();

            uint64_t getSize();
            VEngineBufferType getType();
        };
    }
}
