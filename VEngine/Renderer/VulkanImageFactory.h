#pragma once
#include "../Interface/Renderer/Enums.h"
#include "../Interface/Renderer/ImageFactoryInterface.h"

namespace VEngine
{
    namespace FileSystem
    {
        class MediaInterface;
    }

    namespace Renderer
    {
        namespace Internal
        {
            class VulkanDevice;
        }

        class VulkanImageFactory : public ImageFactoryInterface
        {
        public:
            VulkanImageFactory(Internal::VulkanDevice* device, FileSystem::MediaInterface * media);
            ~VulkanImageFactory();

            virtual ImageInterface* build(uint32_t width, uint32_t height, uint32_t depth,
                VEngineImageFormat format, VEngineImageUsage usage, VEngineImageAspect aspect, VEngineImageLayout layout) override;

            virtual ImageInterface* build(uint32_t width, uint32_t height, uint32_t depth,
                VEngineImageFormat format, VEngineImageUsage usage) override;

            virtual ImageInterface* build(uint32_t width, uint32_t height,
                VEngineImageFormat format, VEngineImageUsage usage, VEngineImageAspect aspect, VEngineImageLayout layout) override;

            virtual ImageInterface* build(uint32_t width, uint32_t height,
                VEngineImageFormat format, VEngineImageUsage usage) override;

            virtual ImageInterface* build(uint32_t width, uint32_t height, VEngineImageFormat format, int usage) override;

            virtual ImageInterface* buildMipmapped(uint32_t width, uint32_t height, uint32_t depth,
                VEngineImageFormat format, VEngineImageUsage usage, VEngineImageAspect aspect, VEngineImageLayout layout) override;

            virtual ImageInterface* buildMipmapped(uint32_t width, uint32_t height, uint32_t depth,
                VEngineImageFormat format, VEngineImageUsage usage) override;

            virtual ImageInterface* buildMipmapped(uint32_t width, uint32_t height,
                VEngineImageFormat format, VEngineImageUsage usage, VEngineImageAspect aspect, VEngineImageLayout layout) override;

            virtual ImageInterface* buildMipmapped(uint32_t width, uint32_t height,
                VEngineImageFormat format, VEngineImageUsage usage) override;

            virtual ImageInterface* buildMipmapped(uint32_t width, uint32_t height, VEngineImageFormat format, int usage) override;

            virtual ImageInterface* build(std::string mediakey) override;

            virtual ImageInterface* build(uint32_t width, uint32_t height, uint32_t channelCount, void * data) override;

            virtual bool resolveIsDepthBuffer(VEngineImageFormat format) override;

            static bool internalResolveIsDepthBuffer(VEngineImageFormat format);


        private:
            Internal::VulkanDevice * device;
            FileSystem::MediaInterface * media;
        };


    }
}