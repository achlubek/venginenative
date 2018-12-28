#pragma once
#include <cstdint>

namespace VEngine
{
    namespace Renderer
    {
        class ImageInterface;
        class GenericBufferInterface;

        class DescriptorSetInterface
        {
        public:
            virtual void bindImageViewSampler(uint32_t binding, ImageInterface* img) = 0;
            virtual void bindImageStorage(uint32_t binding, ImageInterface* img) = 0;
            virtual void bindBuffer(uint32_t binding, GenericBufferInterface* buffer) = 0;
        };

    }
}