#include "stdafx.h"
#include "VulkanShaderModule.h"
#include "VulkanToolkit.h"
#include <iostream>
#include <fstream>
#include <vulkan.h>

VulkanShaderModule::VulkanShaderModule(VulkanToolkit * vulkan, VulkanShaderModuleType type, std::string path)
    : vulkan(vulkan), type(type)
{
    std::ifstream file(path, std::ios::ate | std::ios::binary);

    if (!file.is_open()) { 
        printf("failed to open file!");
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
VkShaderModule VulkanShaderModule::getHandle()
{
    return handle;
}
VulkanShaderModuleType VulkanShaderModule::getType()
{
    return type;
}