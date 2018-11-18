#pragma once 
#include "../VulkanAttachment.h"
#include "VulkanSingleAllocation.h"
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
            struct ImageData {
            public:
                int width, height, channelCount;
                void* data;
            };

            class VulkanInternalImage
            {
            public:
                VulkanInternalImage(VulkanDevice * vulkan, uint32_t width, uint32_t height, uint32_t depth, VkFormat format, VkImageTiling tiling,
                    VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImageAspectFlags aspectFlags,
                    VkImageLayout initialLayout, bool mipmapped);

                VulkanInternalImage(VulkanDevice * device, VkFormat format, VkImage imageHandle, VkImageView viewHandle);

                VulkanInternalImage(VulkanDevice * device, FileSystem::Media* media, std::string mediakey);

                VulkanInternalImage(VulkanDevice * device, uint32_t width, uint32_t height, uint32_t channelCount, void* data);

                ~VulkanInternalImage();

                VkSampler getSampler();
                VkImageView getImageView();
                VkImageView getFirstMipmapImageView();
                VkFormat getFormat();
                void regenerateMipmaps();

            private:
                VulkanDevice * device;
                VkImage image;
                VulkanSingleAllocation imageMemory;
                VkImageView imageView;
                VkImageView firstMipmapImageView;

                uint32_t width;
                uint32_t height;
                uint32_t depth;
                uint32_t mipLevels = 1;
                VkFormat format;
                VkImageTiling tiling;
                VkImageUsageFlags usage;
                VkMemoryPropertyFlags properties;
                VkImageAspectFlags aspectFlags;
                VkImageLayout initialLayout;
                VkImageLayout currentLayout;
                VkSampler sampler;
                bool samplerCreated = false;
                void initalize();


                ImageData readFileImageData(std::string path);
                void createTexture(const ImageData &img, VkFormat format);
                void generateMipmaps(VkImage image, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);

                VkCommandBuffer beginSingleTimeCommands();
                void endSingleTimeCommands(VkCommandBuffer commandBuffer);
                void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
                void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
                void transitionImageLayoutExistingCommandBuffer(VkCommandBuffer buffer, uint32_t mipmap, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout
                    , VkPipelineStageFlags sourceStage, VkPipelineStageFlags destinationStage);
                void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
            };
        }
    }
}