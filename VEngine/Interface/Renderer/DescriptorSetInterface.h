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
            void bindImageViewSampler(uint32_t binding, ImageInterface* img);
            void bindImageStorage(uint32_t binding, ImageInterface* img);
            void bindBuffer(uint32_t binding, GenericBufferInterface* buffer);
        };

    }
}