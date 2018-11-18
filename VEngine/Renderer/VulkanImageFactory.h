#pragma once
#include "VulkanEnums.h"

namespace VEngine
{
    namespace FileSystem
    {
        class Media;
    }

    namespace Renderer
    {
        namespace Internal
        {
            class VulkanDevice;
        }
        class VulkanImage;

        class VulkanImageFactory
        {
        public:
            VulkanImageFactory(Internal::VulkanDevice* device, FileSystem::Media * media);
            ~VulkanImageFactory();

            VulkanImage* build(uint32_t width, uint32_t height, uint32_t depth,
                VulkanImageFormat format, VulkanImageUsage usage, VulkanImageAspect aspect, VulkanImageLayout layout);

            VulkanImage* build(uint32_t width, uint32_t height, uint32_t depth,
                VulkanImageFormat format, VulkanImageUsage usage);

            VulkanImage* build(uint32_t width, uint32_t height,
                VulkanImageFormat format, VulkanImageUsage usage, VulkanImageAspect aspect, VulkanImageLayout layout);

            VulkanImage* build(uint32_t width, uint32_t height,
                VulkanImageFormat format, VulkanImageUsage usage);

            VulkanImage* build(uint32_t width, uint32_t height, VulkanImageFormat format, int usage);

            VulkanImage* buildMipmapped(uint32_t width, uint32_t height, uint32_t depth,
                VulkanImageFormat format, VulkanImageUsage usage, VulkanImageAspect aspect, VulkanImageLayout layout);

            VulkanImage* buildMipmapped(uint32_t width, uint32_t height, uint32_t depth,
                VulkanImageFormat format, VulkanImageUsage usage);

            VulkanImage* buildMipmapped(uint32_t width, uint32_t height,
                VulkanImageFormat format, VulkanImageUsage usage, VulkanImageAspect aspect, VulkanImageLayout layout);

            VulkanImage* buildMipmapped(uint32_t width, uint32_t height,
                VulkanImageFormat format, VulkanImageUsage usage);

            VulkanImage* buildMipmapped(uint32_t width, uint32_t height, VulkanImageFormat format, int usage);

            VulkanImage* build(std::string mediakey);

            VulkanImage* build(uint32_t width, uint32_t height, uint32_t channelCount, void * data);


        private:
            Internal::VulkanDevice * device;
            FileSystem::Media * media;
        };


    }
}