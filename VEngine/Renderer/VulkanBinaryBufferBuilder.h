#pragma once
#include "../Interface/Renderer/BinaryBufferBuilderInterface.h"

namespace VEngine
{
    namespace Renderer
    {
        class VulkanBinaryBufferBuilder : public BinaryBufferBuilderInterface
        {
        public:
            VulkanBinaryBufferBuilder();
            ~VulkanBinaryBufferBuilder();
            virtual void* getPointer() override;
            virtual void emplaceByte(unsigned char byte) override;
            virtual void emplaceInt32(int32_t byte) override;
            virtual void emplaceFloat32(float byte) override;
            virtual void emplaceGeneric(unsigned char* m, int bytes) override;
        private:
            std::vector<unsigned char> buffer;
        };

    }
}