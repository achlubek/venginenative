#pragma once
namespace VEngine
{
    namespace Renderer
    {
        namespace Internal
        {
            struct SwapChainSupportDetails {
                VkSurfaceCapabilitiesKHR capabilities;
                std::vector<VkSurfaceFormatKHR> formats;
                std::vector<VkPresentModeKHR> presentModes;
            };
            class VulkanDevice;
            class VulkanSwapChain
            {
            public:
                VulkanSwapChain(VulkanDevice * device, uint32_t width, uint32_t height, VkSurfaceKHR surface, GLFWwindow* window, VkPhysicalDevice physicalDevice);
                ~VulkanSwapChain();
                void present(std::vector<VkSemaphore> waitSemaphores, const uint32_t imageIndex);
                size_t getImagesCount();
                VkImage getImage(unsigned int index);
                VkImageView getImageView(unsigned int index);
                VkFormat getImageFormat();
                VkSwapchainKHR getHandle(); // todoo instead implementt image acquire behaviour

            private:

                VulkanDevice * device;
                VkSwapchainKHR swapChain;
                VkSurfaceKHR surface;
                GLFWwindow* window;
                VkPhysicalDevice physicalDevice;
                std::vector<VkImage> swapChainImages;
                std::vector<VkImageView> swapChainImageViews;
                VkFormat swapChainImageFormat;
                VkExtent2D swapChainExtent;

                SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
                VkExtent2D chooseSwapExtent(uint32_t width, uint32_t height, const VkSurfaceCapabilitiesKHR& capabilities);
                VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes);
                VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

                void createSwapChain(uint32_t width, uint32_t height);

            };

        }
    }
}