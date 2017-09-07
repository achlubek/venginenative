#pragma once
struct GLFWwindow;

struct ImageData {
public:
    int width, height, channelCount;
    void* data;
};

class VulkanToolkit
{
public:
    
    static VulkanToolkit* singleton;
    VulkanToolkit();
    ~VulkanToolkit();
    VkInstance instance;
    std::vector<VkPhysicalDevice> physicalDevices;
    std::vector<VkPhysicalDeviceProperties> physicalDevicesProps;
    std::vector<VkPhysicalDeviceFeatures> physicalDevicesFeats;
    std::vector<VkPhysicalDeviceMemoryProperties> physicalDevicesMemoryProps;
    VkDevice device;
    VkPhysicalDevice pdevice;
    GLFWwindow *window;
    VkSurfaceKHR surface;
    VkDebugReportCallbackEXT callback;
    Object3dInfo* fullScreenQuad3dInfo;
    void initialize(int width, int height);
    void createInstance(std::string appname, std::string enginename);
    void setupDebugCallback();
    void loadPhysicalDevices();
    std::vector<VkQueueFamilyProperties> getQueueFamilyProperties(VkPhysicalDevice pdevice);
    void createLogicalDevice(VkPhysicalDevice pdevice, VkDeviceCreateInfo cinfo);
    const std::vector<const char*> validationLayers = {
        "VK_LAYER_LUNARG_standard_validation",
        //"VK_LAYER_LUNARG_api_dump"
    };
    int chosenDeviceId = 0;
    int chosenQFId = 0;

    int windowWidth;
    int windowHeight;

    bool shouldCloseWindow();
    void poolEvents();
    double getExecutionTime();

    VulkanSwapChain* swapChain;
     
    VkCommandPool commandPool;

    VkQueue mainQueue;
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties); 

    ImageData* readFileImageData(std::string path);
    VulkanImage* createTexture(ImageData * img, VkFormat format);

    VkCommandBuffer beginSingleTimeCommands();
    void endSingleTimeCommands(VkCommandBuffer commandBuffer);
    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
    void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
    void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
};
