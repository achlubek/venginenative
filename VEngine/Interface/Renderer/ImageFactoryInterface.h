#pragma once
#include <cstdint>
#include "Enums.h"

namespace VEngine
{
    namespace FileSystem
    {
        class MediaInterface;
    }

    namespace Renderer
    {
        class ImageInterface;

        class ImageFactoryInterface
        {
        public:
            ImageInterface * build(uint32_t width, uint32_t height, uint32_t depth,
                VEngineImageFormat format, VEngineImageUsage usage, VEngineImageAspect aspect, VEngineImageLayout layout);

            ImageInterface* build(uint32_t width, uint32_t height, uint32_t depth,
                VEngineImageFormat format, VEngineImageUsage usage);

            ImageInterface* build(uint32_t width, uint32_t height,
                VEngineImageFormat format, VEngineImageUsage usage, VEngineImageAspect aspect, VEngineImageLayout layout);

            ImageInterface* build(uint32_t width, uint32_t height,
                VEngineImageFormat format, VEngineImageUsage usage);

            ImageInterface* build(uint32_t width, uint32_t height, VEngineImageFormat format, int usage);

            ImageInterface* buildMipmapped(uint32_t width, uint32_t height, uint32_t depth,
                VEngineImageFormat format, VEngineImageUsage usage, VEngineImageAspect aspect, VEngineImageLayout layout);

            ImageInterface* buildMipmapped(uint32_t width, uint32_t height, uint32_t depth,
                VEngineImageFormat format, VEngineImageUsage usage);

            ImageInterface* buildMipmapped(uint32_t width, uint32_t height,
                VEngineImageFormat format, VEngineImageUsage usage, VEngineImageAspect aspect, VEngineImageLayout layout);

            ImageInterface* buildMipmapped(uint32_t width, uint32_t height,
                VEngineImageFormat format, VEngineImageUsage usage);

            ImageInterface* buildMipmapped(uint32_t width, uint32_t height, VEngineImageFormat format, int usage);

            ImageInterface* build(std::string mediakey);

            ImageInterface* build(uint32_t width, uint32_t height, uint32_t channelCount, void * data);
        };


    }
}