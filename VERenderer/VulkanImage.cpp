#include "stdafx.h" 
#include "VulkanToolkit.h" 
#include "VulkanMemoryManager.h" 
#include "VulkanMemoryChunk.h" 

VulkanImage::VulkanImage(VulkanToolkit * ivulkan, uint32_t iwidth, uint32_t iheight, VkFormat iformat, VkImageTiling itiling,
    VkImageUsageFlags iusage, VkMemoryPropertyFlags iproperties, VkImageAspectFlags iaspectFlags,
    VkImageLayout iinitialLayout, bool iisDepthBuffer)
    : vulkan(ivulkan), imageMemory(VulkanSingleAllocation(nullptr,0,0))
{
    width = iwidth;
    height = iheight;
    format = iformat;
    tiling = itiling;
    usage = iusage;
    properties = iproperties;
    aspectFlags = iaspectFlags;
    initialLayout = iinitialLayout;
    isDepthBuffer = iisDepthBuffer;

    VkImageCreateInfo imageInfo = {};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
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
    viewInfo.subresourceRange.levelCount = 1;
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
 : imageMemory(VulkanSingleAllocation(nullptr, 0, 0))
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
   // sampler = new VkSampler();
    VkSamplerCreateInfo samplerInfo = {};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.anisotropyEnable = VK_TRUE;
    samplerInfo.maxAnisotropy = 16;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR; 
    if (vkCreateSampler(vulkan->device, &samplerInfo, nullptr, &sampler) != VK_SUCCESS) {
        throw std::runtime_error("failed to create texture sampler!");
    }
    samplerCreated = true;
    return sampler;
}

VulkanAttachment VulkanImage::getAttachment()
{
    return VulkanAttachment(this, format, VK_SAMPLE_COUNT_1_BIT,
        VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE,
        VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE,
        VK_IMAGE_LAYOUT_UNDEFINED, isPresentReady ? VK_IMAGE_LAYOUT_PRESENT_SRC_KHR  : (isDepthBuffer ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL));
}
