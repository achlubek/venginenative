#include "stdafx.h"
#include "VulkanToolkit.h"
#include "Internal/VulkanDevice.h"

VulkanToolkit::VulkanToolkit(int width, int height, bool enableValidation, std::string windowName)
    : device(new VulkanDevice(width, height, enableValidation, windowName)),
    windowWidth(width), windowHeight(height)
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
