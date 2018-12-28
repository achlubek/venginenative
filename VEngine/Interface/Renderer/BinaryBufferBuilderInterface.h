#pragma once
#include <cstdint>

namespace VEngine
{
    namespace Renderer
    {
        class BinaryBufferBuilderInterface
        {
        public:
            virtual void* getPointer() = 0;
            virtual void emplaceByte(unsigned char byte) = 0;
            virtual void emplaceInt32(int32_t byte) = 0;
            virtual void emplaceFloat32(float byte) = 0;
            virtual void emplaceGeneric(unsigned char* m, int bytes) = 0;
        };

    }
}