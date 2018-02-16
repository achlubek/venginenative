#include "stdafx.h" 
#include "VulkanToolkit.h" 
#include "VulkanMemoryManager.h" 
#include "VulkanMemoryChunk.h" 

VulkanImage::VulkanImage(VulkanToolkit * ivulkan, uint32_t iwidth, uint32_t iheight, VkFormat iformat, VkImageTiling itiling,
    VkImageUsageFlags iusage, VkMemoryPropertyFlags iproperties, VkImageAspectFlags iaspectFlags,
    VkImageLayout iinitialLayout, bool iisDepthBuffer)
    : vulkan(ivulkan)
{
    width = iwidth;
    height = iheight;
    format = iformat;
    tiling = itiling;
    usage = iusage | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    properties = iproperties;
    aspectFlags = iaspectFlags;
    initialLayout = iinitialLayout;
    isDepthBuffer = iisDepthBuffer;
    attachmentBlending = VulkanAttachmentBlending::None;
    unsigned int sz = min(width, height) / 2;
    int mipmaps = 1;
   // for (int32_t i = 1; sz > 1; i++)
    //{
   //     mipmaps++;
   //     sz /= 2;
   // }
    VkImageCreateInfo imageInfo = {};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = mipmaps;
    imageInfo.arrayLayers = 1;
    imageInfo.format = format;
    imageInfo.tiling = tiling;
    imageInfo.initialLayout = initialLayout;
    imageInfo.usage = usage;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateImage(vulkan->device, &imageInfo, nullptr, &image) != VK_SUCCESS) {
        throw std::runtime_error("failed to create image!");
    }

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(vulkan->device, image, &memRequirements);
    /*
    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = vulkan->findMemoryType(memRequirements.memoryTypeBits, properties);*/

    imageMemory = vulkan->memoryManager->bindImageMemory(vulkan->findMemoryType(memRequirements.memoryTypeBits, properties), image, memRequirements.size);
    /*
    if (vkAllocateMemory(vulkan->device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate image memory!");
    }

    vkBindImageMemory(vulkan->device, image, imageMemory, 0);*/

    VkImageViewCreateInfo viewInfo = {};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;
    viewInfo.subresourceRange.aspectMask = aspectFlags;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = mipmaps;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    if (vkCreateImageView(vulkan->device, &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
        throw std::runtime_error("failed to create texture image view!");
    }
    samplerCreated = false;
}

VulkanImage::~VulkanImage()
{
    if (samplerCreated) {
        vkDestroySampler(vulkan->device, sampler, nullptr);
    }
    vkDestroyImageView(vulkan->device, imageView, nullptr);
    vkDestroyImage(vulkan->device, image, nullptr);
    imageMemory.free();
   // vkFreeMemory(vulkan->device, imageMemory, nullptr);
}

VulkanImage::VulkanImage(VkFormat iformat, VkImage imageHandle, VkImageView viewHandle)
{
    iformat = format;
    image = imageHandle;
    imageView = viewHandle; 
    isDepthBuffer = false;
    samplerCreated = false;
}

VkSampler VulkanImage::getSampler()
{
    if (samplerCreated) {
        return sampler;
    }
    unsigned int sz = min(width, height) / 2;
    int mipmaps = 1;
    for (int32_t i = 1; sz > 1; i++)
    {
     //   mipmaps++;
        sz /= 2;
    }
   // sampler = new VkSampler();
    VkSamplerCreateInfo samplerInfo = {};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.anisotropyEnable = VK_FALSE;
    samplerInfo.maxAnisotropy = 1;// 16;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.maxLod = mipmaps;
    if (vkCreateSampler(vulkan->device, &samplerInfo, nullptr, &sampler) != VK_SUCCESS) {
        throw std::runtime_error("failed to create texture sampler!");
    }
    samplerCreated = true;
    return sampler;
}

VulkanAttachment VulkanImage::getAttachment()
{
    auto att = VulkanAttachment(this, format, VK_SAMPLE_COUNT_1_BIT,
        VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE,
        VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE,
        VK_IMAGE_LAYOUT_UNDEFINED, isPresentReady ? VK_IMAGE_LAYOUT_PRESENT_SRC_KHR  : (isDepthBuffer ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL));
    att.blending = attachmentBlending;
    return att;
}

void VulkanImage::generateMipMaps()
{
    return;

    VkCommandBuffer blitCmd = vulkan->beginSingleTimeCommands();

    vulkan->transitionImageLayoutExistingCommandBuffer(blitCmd, 0, image, format, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, 
        VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);

    // Copy down mips from n-1 to n
    unsigned int sz = min(width, height) / 2;
    for (int32_t i = 1; sz > 1; i++)
    {
        sz /= 2;
        VkImageBlit imageBlit{};

        // Source
        imageBlit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageBlit.srcSubresource.layerCount = 1;
        imageBlit.srcSubresource.mipLevel = i - 1;
        imageBlit.srcOffsets[1].x = int32_t(width >> (i - 1));
        imageBlit.srcOffsets[1].y = int32_t(height >> (i - 1));
        imageBlit.srcOffsets[1].z = 1;

        // Destination
        imageBlit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageBlit.dstSubresource.layerCount = 1;
        imageBlit.dstSubresource.mipLevel = i;
        imageBlit.dstOffsets[1].x = int32_t(width >> i);
        imageBlit.dstOffsets[1].y = int32_t(height >> i);
        imageBlit.dstOffsets[1].z = 1;

        VkImageSubresourceRange mipSubRange = {};
        mipSubRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        mipSubRange.baseMipLevel = i;
        mipSubRange.levelCount = 1;
        mipSubRange.layerCount = 1;

        // Transiton current mip level to transfer dest
        vulkan->transitionImageLayoutExistingCommandBuffer(
            blitCmd, i,
            image,
            format, 
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
            VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);

        // Blit from previous level
        vkCmdBlitImage(
            blitCmd,
            image,
            VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            image,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1,
            &imageBlit,
            VK_FILTER_LINEAR);

        // Transiton current mip level to transfer source for read in next iteration
        vulkan->transitionImageLayoutExistingCommandBuffer(
            blitCmd, i,
            image,
            format,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
            VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);
    }
    sz = min(width, height);
    for (int32_t i = 0; sz > 1; i++)
    {
        vulkan->transitionImageLayoutExistingCommandBuffer(
            blitCmd, i,
            image,
            format,
            VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 
            VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
        sz /= 2;
    }


    vulkan->endSingleTimeCommands(blitCmd);

}
