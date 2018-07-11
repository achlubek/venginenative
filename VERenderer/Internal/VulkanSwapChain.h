#pragma once
#include <vulkan.h>
#include <GLFW/glfw3.h>
struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};
class VulkanDevice;
class VulkanSwapChain
{
public:
    VulkanSwapChain(VulkanDevice * device, int width, int height, VkSurfaceKHR surface, GLFWwindow* window, VkPhysicalDevice physicalDevice);
    ~VulkanSwapChain();
    void present(std::vector<VkSemaphore> waitSemaphores, const uint32_t imageIndex);

private:

    VulkanDevice * device;
    VkSwapchainKHR swapChain;
    VkSurfaceKHR surface;
    GLFWwindow* window;
    VkPhysicalDevice physicalDevice;
    std::vector<VkImage> swapChainImages;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;

    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
    VkExtent2D chooseSwapExtent(int width, int height, const VkSurfaceCapabilitiesKHR& capabilities);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes);
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

    void createSwapChain(int width, int height);

    std::vector<VkImageView> swapChainImageViews;
};

