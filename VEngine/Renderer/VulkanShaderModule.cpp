#include "stdafx.h"
#include "VulkanShaderModule.h"
#include "Internal/VulkanDevice.h"
#include <iostream>
#include <fstream>
#include <cstring>

namespace VEngine
{
    namespace Renderer
    {
        using namespace VEngine::Renderer::Internal;

        VulkanShaderModule::VulkanShaderModule(VulkanDevice * device, VulkanShaderModuleType type, std::string path)
            : device(device), type(type)
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
            std::memcpy(codeAligned.data(), buffer.data(), buffer.size());

            createInfo.pCode = codeAligned.data();

            vkCreateShaderModule(device->getDevice(), &createInfo, nullptr, &handle);
        }

        VulkanShaderModule::~VulkanShaderModule()
        {
            vkDestroyShaderModule(device->getDevice(), handle, nullptr);
        }
        VkShaderModule VulkanShaderModule::getHandle()
        {
            return handle;
        }
        VulkanShaderModuleType VulkanShaderModule::getType()
        {
            return type;
        }
    }
}
