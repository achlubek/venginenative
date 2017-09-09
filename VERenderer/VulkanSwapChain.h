#pragma once
struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};
class VulkanToolkit;
class VulkanSwapChain
{
public:
    VulkanSwapChain(VulkanToolkit * vulkan, int width, int height);
    ~VulkanSwapChain();

    VulkanToolkit * vulkan;
    VkSwapchainKHR swapChain;
    std::vector<VkImage> swapChainImages;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;

    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
    VkExtent2D chooseSwapExtent(int width, int height, const VkSurfaceCapabilitiesKHR& capabilities);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes);
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

    void createSwapChain(int width, int height);

    std::vector<VkImageView> swapChainImageViews;
    void present(std::vector<VkSemaphore> waitSemaphores, const uint32_t imageIndex);
};

