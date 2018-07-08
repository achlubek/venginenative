#include "stdafx.h" 
#include "VulkanInternalImage.h" 
#include "VulkanDevice.h" 
#include "VulkanMemoryManager.h" 
#include "VulkanMemoryChunk.h" 

VulkanInternalImage::VulkanInternalImage(VulkanDevice * device, uint32_t iwidth, uint32_t iheight, VkFormat iformat, VkImageTiling itiling,
    VkImageUsageFlags iusage, VkMemoryPropertyFlags iproperties, VkImageAspectFlags iaspectFlags,
    VkImageLayout iinitialLayout, bool iisDepthBuffer)
    : device(device),
    width(iwidth),
    height(iheight),
    depth(1),
    arrayLayers(1),
    format(iformat),
    tiling(itiling),
    usage(iusage | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT),
    properties(iproperties),
    aspectFlags(iaspectFlags),
    initialLayout(iinitialLayout),
    isDepthBuffer(iisDepthBuffer),
    clearColor({ 0.0f, 0.0f, 0.0f, 0.0f }),
    attachmentBlending(VulkanAttachmentBlending::None)
{
    initalize();
}

VulkanInternalImage::VulkanInternalImage(VulkanDevice * device, uint32_t iwidth, uint32_t iheight, uint32_t idepth, VkFormat iformat, VkImageTiling itiling,
    VkImageUsageFlags iusage, VkMemoryPropertyFlags iproperties, VkImageAspectFlags iaspectFlags,
    VkImageLayout iinitialLayout, bool iisDepthBuffer)
    : device(device),
    width(iwidth),
    height(iheight),
    depth(idepth),
    arrayLayers(1),
    format(iformat),
    tiling(itiling),
    usage(iusage | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT),
    properties(iproperties),
    aspectFlags(iaspectFlags),
    initialLayout(iinitialLayout),
    isDepthBuffer(iisDepthBuffer),
    clearColor({ 0.0f, 0.0f, 0.0f, 0.0f }),
    attachmentBlending(VulkanAttachmentBlending::None)
{
    initalize();
}

VulkanInternalImage::VulkanInternalImage(VulkanDevice * device, uint32_t iwidth, uint32_t iheight, uint32_t idepth, uint32_t iarrayLayers, VkFormat iformat, VkImageTiling itiling,
    VkImageUsageFlags iusage, VkMemoryPropertyFlags iproperties, VkImageAspectFlags iaspectFlags,
    VkImageLayout iinitialLayout, bool iisDepthBuffer)
    : device(device),
    width(iwidth),
    height(iheight),
    depth(idepth),
    arrayLayers(iarrayLayers),
    format(iformat),
    tiling(itiling),
    usage(iusage | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT),
    properties(iproperties),
    aspectFlags(iaspectFlags),
    initialLayout(iinitialLayout),
    isDepthBuffer(iisDepthBuffer),
    clearColor({ 0.0f, 0.0f, 0.0f, 0.0f }),
    attachmentBlending(VulkanAttachmentBlending::None)
{
    initalize();
}

VulkanInternalImage::~VulkanInternalImage()
{
    if (samplerCreated) {
        vkDestroySampler(device->getDevice(), sampler, nullptr);
    }
    vkDestroyImageView(device->getDevice(), imageView, nullptr);
    vkDestroyImage(device->getDevice(), image, nullptr);
    imageMemory.free();
    // vkFreeMemory(device->getDevice(), imageMemory, nullptr);
}

void VulkanInternalImage::initalize()
{
    VkImageCreateInfo imageInfo = {};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    if (depth == 1) {
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
    }
    else {
        imageInfo.imageType = VK_IMAGE_TYPE_3D;
    }
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = depth;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = arrayLayers;
    imageInfo.format = format;
    imageInfo.tiling = tiling;
    imageInfo.initialLayout = initialLayout;
    imageInfo.usage = usage;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateImage(device->getDevice(), &imageInfo, nullptr, &image) != VK_SUCCESS) {
        throw std::runtime_error("failed to create image!");
    }

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(device->getDevice(), image, &memRequirements);

    imageMemory = device->getMemoryManager()->bindImageMemory(device->findMemoryType(memRequirements.memoryTypeBits, properties), image, memRequirements.size);

    VkImageViewCreateInfo viewInfo = {};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;

    if (depth == 1) {
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    }
    else {
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_3D;
    }
    viewInfo.format = format;
    viewInfo.subresourceRange.aspectMask = aspectFlags;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = arrayLayers;

    if (vkCreateImageView(device->getDevice(), &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
        throw std::runtime_error("failed to create texture image view!");
    }
    samplerCreated = false;
}

VulkanInternalImage::VulkanInternalImage(VkFormat iformat, VkImage imageHandle, VkImageView viewHandle)
{
    iformat = format;
    image = imageHandle;
    imageView = viewHandle;
    isDepthBuffer = false;
    samplerCreated = false;
    clearColor = { 0.0f, 0.0f, 0.0f, 0.0f };
}

VkSampler VulkanInternalImage::getSampler()
{
    if (samplerCreated) {
        return sampler;
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
    samplerInfo.maxLod = 1;
    if (vkCreateSampler(device->getDevice(), &samplerInfo, nullptr, &sampler) != VK_SUCCESS) {
        throw std::runtime_error("failed to create texture sampler!");
    }
    samplerCreated = true;
    return sampler;
}

VulkanAttachment VulkanInternalImage::getAttachment()
{
    auto att = VulkanAttachment(this, format, VK_SAMPLE_COUNT_1_BIT,
        VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE,
        VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE,
        VK_IMAGE_LAYOUT_UNDEFINED, isPresentReady ? VK_IMAGE_LAYOUT_PRESENT_SRC_KHR : (isDepthBuffer ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL));
    att.blending = attachmentBlending;
    att.clearColor = clearColor;
    att.clear = clear;
    return att;
}

void VulkanInternalImage::generateMipMaps()
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