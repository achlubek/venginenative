#pragma once
 

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
	void initialize();
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

	VulkanSwapChain* swapChain;
	 
    VkCommandPool commandPool;

    VkQueue mainQueue;
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
	//VulkanMesh3d* mesh;
	//VulkanMesh3d* mesh2;
	//VulkanMesh3d* postprocessmesh;

	/*
	Descriptor set layout:
	 This is where we specity that pipeline for example takes 1 ubo and 1 sampler, define the slots to where bind later
	 this is a layout WHAT to take from the POOL of available descriptors

	 Descriptor pool:
	 This is a whole pool of available descriptors, like, imagine a descriptor being an orange and you want to serve 100 people with 2 oranges
	 then you need a pool with 200 oranges, the "2 orange pack" is the descriptor set layout and every pack given to someone is descriptor set


	 TACTIC for meshes:
	 setup a class named DescriptorSetProvider
	 this is an singleton class
	 this class allocate array of pools, evey pool with pretty high limits
	 then it define a constant set of descriotor set layours, this is know beforehand and why to not hardcode it (?)

	 ///////////////////

	 The pipeline now is:

	 INIT:
	 Create Image - 'Color' Image width x height rgba8unorm - with usage COLOR ATTACHMENT and SAMPLING TARGET
	 Create Image - 'Depth' Image width x height df32 - with usage DEPTH ATTACHMENT
	 Create Rendering Stage - 'MESH' - specifying shaders to use, framebuffer using images
	 What I propose:

	 RENDER STAGE 'MESH' {
		shaders: here shares to use triangle.frag/vert
		viewport: x;y
		descriptorsmanager: here the manager instance may be new
		more like i would like to have render stage to be set up and then compiled
		like
		stage = new RenderStage();
		stage.setshaders(shadermodules)
		stage.setviewport(xy)
		stage.attach(image, aspect);

		stage.compile() generates command buffer, framebuffer, pipeline and such
	 }
	*/ 
	 

	ImageData* readFileImageData(std::string path);
	VulkanImage createTexture(std::string path);
	//Image textureImage;

	VkCommandBuffer beginSingleTimeCommands();
	void endSingleTimeCommands(VkCommandBuffer commandBuffer);
	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
	void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
	void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
};
