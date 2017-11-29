#include "stdafx.h"

VulkanShaderModule::VulkanShaderModule(VulkanToolkit * ivulkan, std::string path)
    : vulkan(ivulkan)
{
    std::ifstream file(path, std::ios::ate | std::ios::binary);

    if (!file.is_open()) { 
        throw std::runtime_error("failed to open file!");
    }

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = buffer.size();

    std::vector<uint32_t> codeAligned(buffer.size() / 4 + 1);
    memcpy(codeAligned.data(), buffer.data(), buffer.size());

    createInfo.pCode = codeAligned.data();

    vkCreateShaderModule(vulkan->device, &createInfo, nullptr, &handle);
}

VulkanShaderModule::~VulkanShaderModule()
{
    vkDestroyShaderModule(vulkan->device, handle, nullptr);
}

VkPipelineShaderStageCreateInfo VulkanShaderModule::createShaderStage(VkShaderStageFlagBits type, const char* entrypoint)
{
    VkPipelineShaderStageCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    info.stage = type;
    info.module = handle;
    info.pName = entrypoint;
    return info;
}
