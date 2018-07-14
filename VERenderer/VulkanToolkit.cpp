#include "stdafx.h"
#include "VulkanToolkit.h"
#include "Internal/VulkanDevice.h"

#include "Object3dInfoFactory.h"
#include "VulkanShaderFactory.h"
#include "VulkanDescriptorSetLayoutFactory.h"
#include "VulkanRenderStageFactory.h"
#include "VulkanComputeStageFactory.h"
#include "VulkanBufferFactory.h"
#include "VulkanImageFactory.h"
#include "VulkanSwapChainOutputFactory.h"

VulkanToolkit::VulkanToolkit(int width, int height, bool enableValidation, std::string windowName)
    : device(new VulkanDevice(width, height, enableValidation, windowName)),
    windowWidth(width), windowHeight(height),
    object3dInfoFactory(new Object3dInfoFactory(device)),
    vulkanShaderFactory(new VulkanShaderFactory(device)),
    vulkanDescriptorSetLayoutFactory(new VulkanDescriptorSetLayoutFactory(device)),
    vulkanRenderStageFactory(new VulkanRenderStageFactory(device)),
    vulkanComputeStageFactory(new VulkanComputeStageFactory(device)),
    vulkanBufferFactory(new VulkanBufferFactory(device)),
    vulkanImageFactory(new VulkanImageFactory(device)),
    vulkanSwapChainOutputFactory(new VulkanSwapChainOutputFactory(device))
{ 
}

VulkanToolkit::~VulkanToolkit()
{
    delete device;
}

bool VulkanToolkit::shouldCloseWindow()
{
    return device->shouldCloseWindow();
}

void VulkanToolkit::poolEvents()
{
    device->poolEvents();
}

double VulkanToolkit::getExecutionTime()
{
    return device->getExecutionTime();
}

VulkanDevice * VulkanToolkit::getDevice()
{
    return device;
}

Object3dInfoFactory * VulkanToolkit::getObject3dInfoFactory()
{
    return object3dInfoFactory;
}

VulkanShaderFactory * VulkanToolkit::getVulkanShaderFactory()
{
    return vulkanShaderFactory;
}

VulkanDescriptorSetLayoutFactory * VulkanToolkit::getVulkanDescriptorSetLayoutFactory()
{
    return vulkanDescriptorSetLayoutFactory;
}

VulkanRenderStageFactory * VulkanToolkit::getVulkanRenderStageFactory()
{
    return vulkanRenderStageFactory;
}

VulkanComputeStageFactory * VulkanToolkit::getVulkanComputeStageFactory()
{
    return vulkanComputeStageFactory;
}

VulkanBufferFactory * VulkanToolkit::getVulkanBufferFactory()
{
    return vulkanBufferFactory;
}

VulkanImageFactory * VulkanToolkit::getVulkanImageFactory()
{
    return vulkanImageFactory;
}

VulkanSwapChainOutputFactory * VulkanToolkit::getVulkanSwapChainOutputFactory()
{
    return vulkanSwapChainOutputFactory;
}
