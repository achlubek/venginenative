#include "stdafx.h" 
#include "VulkanInternalImage.h" 
#include "VulkanDevice.h" 
#include "VulkanMemoryManager.h" 
#include "VulkanMemoryChunk.h" 


VulkanInternalImage::VulkanInternalImage(VulkanDevice * device, uint32_t iwidth, uint32_t iheight, uint32_t idepth, VkFormat iformat, VkImageTiling itiling,
    VkImageUsageFlags iusage, VkMemoryPropertyFlags iproperties, VkImageAspectFlags iaspectFlags,
    VkImageLayout iinitialLayout)
    : device(device),
    width(iwidth),
    height(iheight),
    depth(idepth),
    format(iformat),
    tiling(itiling),
    usage(iusage | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT),
    properties(iproperties),
    aspectFlags(iaspectFlags),
    initialLayout(iinitialLayout)
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
    imageInfo.arrayLayers = 1;
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
    viewInfo.subresourceRange.layerCount = 1;

    if (vkCreateImageView(device->getDevice(), &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
        throw std::runtime_error("failed to create texture image view!");
    }
    samplerCreated = false;
}

VulkanInternalImage::VulkanInternalImage(VulkanDevice * device, VkFormat format, VkImage image, VkImageView imageView)
    : device(device), format(format), image(image), imageView(imageView)
{
    samplerCreated = false;
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

VkImageView VulkanInternalImage::getImageView()
{
    return imageView;
}
