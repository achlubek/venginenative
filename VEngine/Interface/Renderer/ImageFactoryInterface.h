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
            virtual ImageInterface * build(uint32_t width, uint32_t height, uint32_t depth,
                VEngineImageFormat format, VEngineImageUsage usage, VEngineImageAspect aspect, VEngineImageLayout layout) = 0;

            virtual ImageInterface* build(uint32_t width, uint32_t height, uint32_t depth,
                VEngineImageFormat format, VEngineImageUsage usage) = 0;

            virtual ImageInterface* build(uint32_t width, uint32_t height,
                VEngineImageFormat format, VEngineImageUsage usage, VEngineImageAspect aspect, VEngineImageLayout layout) = 0;

            virtual ImageInterface* build(uint32_t width, uint32_t height,
                VEngineImageFormat format, VEngineImageUsage usage) = 0;

            virtual ImageInterface* build(uint32_t width, uint32_t height, VEngineImageFormat format, int usage) = 0;

            virtual ImageInterface* buildMipmapped(uint32_t width, uint32_t height, uint32_t depth,
                VEngineImageFormat format, VEngineImageUsage usage, VEngineImageAspect aspect, VEngineImageLayout layout) = 0;

            virtual ImageInterface* buildMipmapped(uint32_t width, uint32_t height, uint32_t depth,
                VEngineImageFormat format, VEngineImageUsage usage) = 0;

            virtual ImageInterface* buildMipmapped(uint32_t width, uint32_t height,
                VEngineImageFormat format, VEngineImageUsage usage, VEngineImageAspect aspect, VEngineImageLayout layout) = 0;

            virtual ImageInterface* buildMipmapped(uint32_t width, uint32_t height,
                VEngineImageFormat format, VEngineImageUsage usage) = 0;

            virtual ImageInterface* buildMipmapped(uint32_t width, uint32_t height, VEngineImageFormat format, int usage) = 0;

            virtual ImageInterface* build(std::string mediakey) = 0;

            virtual ImageInterface* build(uint32_t width, uint32_t height, uint32_t channelCount, void * data) = 0;

            virtual bool resolveIsDepthBuffer(VEngineImageFormat format) = 0;
        };


    }
}