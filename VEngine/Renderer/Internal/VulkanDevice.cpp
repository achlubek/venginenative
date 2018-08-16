#include "stdafx.h"
#include "VulkanDevice.h"
#include <GLFW/glfw3.h>
#include <iostream>

namespace VEngine
{
    namespace Renderer
    {
        namespace Internal
        {

            VulkanDevice::VulkanDevice(int width, int height, bool enableValidation, std::string windowName)
            {
                windowWidth = width;
                windowHeight = height;
                createInstance(windowName, windowName, enableValidation);
                loadPhysicalDevices();
                for (int i = 0; i < physicalDevices.size(); i++) {
                    auto props = getQueueFamilyProperties(physicalDevices[i]);
                    for (int g = 0; g < props.size(); g++) {
                        auto qfamily = props[g];
                        if (qfamily.queueCount > 0 && qfamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                            chosenDeviceId = i;
                            chosenQFId = g;
                            break;
                        }
                    }
                }

                VkPhysicalDeviceFeatures requiredFeatures = {};
                requiredFeatures.tessellationShader = VK_FALSE;
                requiredFeatures.geometryShader = VK_FALSE;
                requiredFeatures.depthClamp = VK_TRUE;
                requiredFeatures.samplerAnisotropy = VK_FALSE; // I will need to recheck this mipmapping hell some time
                requiredFeatures.shaderStorageImageExtendedFormats = VK_TRUE;
                requiredFeatures.independentBlend = VK_TRUE;
                requiredFeatures.fragmentStoresAndAtomics = VK_TRUE;
                requiredFeatures.vertexPipelineStoresAndAtomics = VK_TRUE;

                const std::vector<float> priorities = { 1.0f, 0.0f };
                const VkDeviceQueueCreateInfo deviceQueueCreateInfo =
                {
                    VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,     // sType
                    nullptr,                                        // pNext
                    0,                                              // flags
                    chosenQFId,                                     // queueFamilyIndex
                    2,                                              // queueCount
                    priorities.data()                                         // pQueuePriorities
                };
                std::vector<const char*> deviceExtensions = {
                    VK_KHR_SWAPCHAIN_EXTENSION_NAME
                };
                VkDeviceCreateInfo createInfo = {};
                createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

                createInfo.pQueueCreateInfos = &deviceQueueCreateInfo;
                createInfo.queueCreateInfoCount = 1;

                createInfo.pEnabledFeatures = &requiredFeatures;

                createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
                createInfo.ppEnabledExtensionNames = deviceExtensions.data();

                if (enableValidation) {
                    createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
                    createInfo.ppEnabledLayerNames = validationLayers.data();
                }
                else {
                    createInfo.enabledLayerCount = 0;
                }

                pdevice = physicalDevices[chosenDeviceId];
                createLogicalDevice(physicalDevices[chosenDeviceId], createInfo);


                vkGetDeviceQueue(device, chosenQFId, 0, &mainQueue);
                vkGetDeviceQueue(device, chosenQFId, 1, &secondaryQueue);

                memoryManager = new VulkanMemoryManager(this);



                VkCommandPoolCreateInfo poolInfo = {};
                poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
                poolInfo.queueFamilyIndex = 0;
                poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

                if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
                    throw std::runtime_error("failed to create command pool!");
                }

                if (width > 0 && height > 0) {
                    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
                    window = glfwCreateWindow(width, height, windowName.c_str(), NULL, NULL);

                    VkResult result;
                    result = glfwCreateWindowSurface(instance, window, NULL, &surface);

                    VkBool32 supported = false;
                    vkGetPhysicalDeviceSurfaceSupportKHR(pdevice, chosenQFId, surface, &supported);
                    swapChain = new VulkanSwapChain(this, width, height, surface, window, pdevice);
                }
                vkDeviceWaitIdle(device);
            }


            VulkanDevice::~VulkanDevice()
            {
                vkDestroyCommandPool(device, commandPool, nullptr);
                safedelete(swapChain);
                safedelete(memoryManager);
                vkDestroySurfaceKHR(instance, surface, nullptr);
                glfwDestroyWindow(window);
                vkDestroyDevice(device, nullptr);
                vkDestroyInstance(instance, nullptr);
            }

            VkDevice VulkanDevice::getDevice()
            {
                return device;
            }

            VkQueue VulkanDevice::getMainQueue()
            {
                return mainQueue;
            }

            VkCommandPool VulkanDevice::getCommandPool()
            {
                return commandPool;
            }

            static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
                VkDebugReportFlagsEXT flags,
                VkDebugReportObjectTypeEXT objType,
                uint64_t obj,
                size_t location,
                int32_t code,
                const char* layerPrefix,
                const char* msg,
                void* userData) {
                
                std::cout << "VALIDATION (Layer " << layerPrefix << ", obj " << obj << ", location " << location << ", code " << code << ":\n" << msg << "\n\n";

                return VK_FALSE;
            }

            void VulkanDevice::createInstance(std::string appname, std::string enginename, bool enableValidation)
            {
                glfwInit();
                VkApplicationInfo appInfo = {};
                appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
                appInfo.pApplicationName = appname.c_str();
                appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
                appInfo.pEngineName = enginename.c_str();
                appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
                appInfo.apiVersion = VK_API_VERSION_1_0;

                uint32_t count;
                const char** extensions = glfwGetRequiredInstanceExtensions(&count);

                std::vector<const char*> deviceExtensions = {
                    VK_EXT_DEBUG_REPORT_EXTENSION_NAME
                };
                for (uint32_t i = 0; i < count; i++) {
                    const char* str = extensions[i];
                    deviceExtensions.push_back(str);
                }

                VkInstanceCreateInfo createInfo = {};
                createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
                createInfo.pApplicationInfo = &appInfo;
                createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
                createInfo.ppEnabledExtensionNames = deviceExtensions.data();

                if (enableValidation) {
                    createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
                    createInfo.ppEnabledLayerNames = validationLayers.data();
                }
                else {
                    createInfo.enabledLayerCount = 0;
                }

                VkResult result;
                result = vkCreateInstance(&createInfo, nullptr, &instance);
                setupDebugCallback();
            }

            VkResult CreateDebugReportCallbackEXT(VkInstance instance, const VkDebugReportCallbackCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugReportCallbackEXT* pCallback) {
                auto func = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");
                if (func != nullptr) {
                    return func(instance, pCreateInfo, pAllocator, pCallback);
                }
                else {
                    return VK_ERROR_EXTENSION_NOT_PRESENT;
                }
            }

            void VulkanDevice::setupDebugCallback()
            {
                VkDebugReportCallbackCreateInfoEXT createInfo = {};
                createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
                createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
                createInfo.pfnCallback = debugCallback;
                CreateDebugReportCallbackEXT(instance, &createInfo, nullptr, &callback);
            }

            void VulkanDevice::loadPhysicalDevices()
            {
                uint32_t physicalDeviceCount = 0;
                VkResult result;
                result = vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);

                physicalDevices = std::vector<VkPhysicalDevice>();
                physicalDevices.resize(physicalDeviceCount);

                physicalDevicesProps = std::vector<VkPhysicalDeviceProperties>();
                physicalDevicesProps.resize(physicalDeviceCount);

                physicalDevicesFeats = std::vector<VkPhysicalDeviceFeatures>();
                physicalDevicesFeats.resize(physicalDeviceCount);

                physicalDevicesMemoryProps = std::vector<VkPhysicalDeviceMemoryProperties>();
                physicalDevicesMemoryProps.resize(physicalDeviceCount);

                result = vkEnumeratePhysicalDevices(instance,
                    &physicalDeviceCount,
                    &physicalDevices[0]);

                for (uint32_t i = 0; i < physicalDeviceCount; i++) {
                    vkGetPhysicalDeviceProperties(physicalDevices[i], &physicalDevicesProps[i]);
                    vkGetPhysicalDeviceFeatures(physicalDevices[i], &physicalDevicesFeats[i]);
                    vkGetPhysicalDeviceMemoryProperties(physicalDevices[i], &physicalDevicesMemoryProps[i]);
                }
            }

            std::vector<VkQueueFamilyProperties> VulkanDevice::getQueueFamilyProperties(VkPhysicalDevice pdevice)
            {
                uint32_t queueFamilyPropertyCount;
                std::vector<VkQueueFamilyProperties> queueFamilyProperties;

                vkGetPhysicalDeviceQueueFamilyProperties(pdevice, &queueFamilyPropertyCount, nullptr);

                queueFamilyProperties.resize(queueFamilyPropertyCount);

                vkGetPhysicalDeviceQueueFamilyProperties(pdevice, &queueFamilyPropertyCount, queueFamilyProperties.data());
                return queueFamilyProperties;
            }

            void VulkanDevice::createLogicalDevice(VkPhysicalDevice pdevice, VkDeviceCreateInfo cinfo)
            {
                VkResult result = vkCreateDevice(pdevice, &cinfo, nullptr, &device);
            }

            bool VulkanDevice::shouldCloseWindow()
            {
                return glfwWindowShouldClose(window);
            }

            void VulkanDevice::poolEvents()
            {
                glfwPollEvents();
            }

            double VulkanDevice::getExecutionTime()
            {
                return glfwGetTime();
            }

            VulkanMemoryManager * VulkanDevice::getMemoryManager()
            {
                return memoryManager;
            }

            VulkanSwapChain * VulkanDevice::getSwapChain()
            {
                return swapChain;
            }

            GLFWwindow * VulkanDevice::getWindow()
            {
                return window;
            }

            uint32_t VulkanDevice::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
                for (uint32_t i = 0; i < physicalDevicesMemoryProps[chosenDeviceId].memoryTypeCount; i++) {
                    if ((typeFilter & (1 << i)) && (physicalDevicesMemoryProps[chosenDeviceId].memoryTypes[i].propertyFlags & properties) == properties) {
                        return i;
                    }
                }

                throw std::runtime_error("failed to find suitable memory type!");
            }
        }
    }
}