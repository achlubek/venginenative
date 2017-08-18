#include "stdafx.h"

#include "../stb_image.h"
#define STB_IMAGE_IMPLEMENTATION

VulkanToolkit* VulkanToolkit::singleton = nullptr;

VulkanToolkit::VulkanToolkit()
{
	singleton = this;
}

VulkanToolkit::~VulkanToolkit()
{
	vkDestroyInstance(instance, nullptr);
}

void VulkanToolkit::initialize()
{
	createInstance("Teapot", "VEngine");
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
	requiredFeatures.samplerAnisotropy = VK_TRUE;

	const std::vector<float> priorities = { 1.0f };
	const VkDeviceQueueCreateInfo deviceQueueCreateInfo =
	{
		VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,     // sType
		nullptr,                                        // pNext
		0,                                              // flags
		chosenQFId,                                     // queueFamilyIndex
		1,                                              // queueCount
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

	createInfo.enabledExtensionCount = deviceExtensions.size();
	createInfo.ppEnabledExtensionNames = deviceExtensions.data();

	createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
	createInfo.ppEnabledLayerNames = validationLayers.data();

	pdevice = physicalDevices[chosenDeviceId];
	createLogicalDevice(physicalDevices[chosenDeviceId], createInfo);

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	window = glfwCreateWindow(WIDTH, HEIGHT, "VULKAN", NULL, NULL);

	VkResult result;
	result = glfwCreateWindowSurface(instance, window, NULL, &surface);
	
	vkGetDeviceQueue(device, chosenQFId, 0, &mainQueue);

	VkBool32 supported = false;
	vkGetPhysicalDeviceSurfaceSupportKHR(pdevice, chosenQFId, surface, &supported);
	swapChain = new VulkanSwapChain(); 

	VkCommandPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = 0;

	if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create command pool!");
	}

	/*
	mesh = new VulkanMesh3d("../shaders/mrstick.vbo", "../test.png");
	mesh2 = new VulkanMesh3d("../shaders/lucy.vbo", "../test2.png");
	postprocessmesh = new VulkanMesh3d("../shaders/ppplane.vbo", "../test2.png");
	postprocessmesh->texture = colorImage;
	postprocessmesh->descriptorSet = VulkanToolkit::singleton->meshSetManager.generateMesh3dDescriptorSet();
	postprocessmesh->descriptorSet.bindUniformBuffer(0, VulkanToolkit::singleton->uniformBuffer);
	postprocessmesh->descriptorSet.bindImageViewSampler(1, colorImage);
	postprocessmesh->descriptorSet.update();


	meshRenderStage.meshes.push_back(*mesh);
	meshRenderStage.meshes.push_back(*mesh2);

	meshRenderStage.updateCommandBuffer();

	for (int i = 0; i < postProcessRenderStages.size(); i++) {
		postProcessRenderStages[i].meshes.push_back(*postprocessmesh);
		postProcessRenderStages[i].updateCommandBuffer();
	}*/
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
	printf("VALIDATION: %s\n\n", msg);

	return VK_FALSE;
}

void VulkanToolkit::createInstance(std::string appname, std::string enginename)
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
	for (int i = 0; i < count; i++) {
		const char* str = extensions[i];
		deviceExtensions.push_back(str);
	}

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledExtensionCount = deviceExtensions.size();
	createInfo.ppEnabledExtensionNames = deviceExtensions.data();
	createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
	createInfo.ppEnabledLayerNames = validationLayers.data();

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

void VulkanToolkit::setupDebugCallback()
{
	VkDebugReportCallbackCreateInfoEXT createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
	createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
	createInfo.pfnCallback = debugCallback;
	CreateDebugReportCallbackEXT(instance, &createInfo, nullptr, &callback);
}

void VulkanToolkit::loadPhysicalDevices()
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
	
	for (int i = 0; i < physicalDeviceCount; i++) {
		vkGetPhysicalDeviceProperties(physicalDevices[i], &physicalDevicesProps[i]);
		vkGetPhysicalDeviceFeatures(physicalDevices[i], &physicalDevicesFeats[i]);
		vkGetPhysicalDeviceMemoryProperties(physicalDevices[i], &physicalDevicesMemoryProps[i]);
	}
}

std::vector<VkQueueFamilyProperties> VulkanToolkit::getQueueFamilyProperties(VkPhysicalDevice pdevice)
{
	uint32_t queueFamilyPropertyCount;
	std::vector<VkQueueFamilyProperties> queueFamilyProperties;
	VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties;

	vkGetPhysicalDeviceQueueFamilyProperties(pdevice, &queueFamilyPropertyCount, nullptr);

	queueFamilyProperties.resize(queueFamilyPropertyCount);

	vkGetPhysicalDeviceQueueFamilyProperties(pdevice, &queueFamilyPropertyCount, queueFamilyProperties.data());
	return queueFamilyProperties;
}

void VulkanToolkit::createLogicalDevice(VkPhysicalDevice pdevice, VkDeviceCreateInfo cinfo)
{
	VkResult result = vkCreateDevice(pdevice, &cinfo, nullptr, &device);
}

uint32_t VulkanToolkit::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
	for (uint32_t i = 0; i < physicalDevicesMemoryProps[chosenDeviceId].memoryTypeCount; i++) {
		if ((typeFilter & (1 << i)) && (physicalDevicesMemoryProps[chosenDeviceId].memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}

	throw std::runtime_error("failed to find suitable memory type!");
}


ImageData * VulkanToolkit::readFileImageData(std::string path)
{
	ImageData * d = new ImageData();
	d->data = stbi_load(path.c_str(), &d->width, &d->height, &d->channelCount, STBI_rgb_alpha);
	return d;
}

VulkanImage VulkanToolkit::createTexture(std::string path)
{
	ImageData * img = readFileImageData(path);
	VulkanGenericBuffer stagingBuffer = VulkanGenericBuffer(img->width * img->height * 4); //  always rgba for now
	void* mappoint;
	stagingBuffer.map(0, img->width * img->height * 4, &mappoint);
	memcpy(mappoint, img->data, static_cast<size_t>(img->width * img->height * 4));
	stagingBuffer.unmap();
	stbi_image_free(img->data);

	VulkanImage res = VulkanImage(img->width, img->height, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED, false);

	transitionImageLayout(res.image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	copyBufferToImage(stagingBuffer.buffer, res.image, static_cast<uint32_t>(img->width), static_cast<uint32_t>(img->height));
	transitionImageLayout(res.image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	return res;
}

VkCommandBuffer VulkanToolkit::beginSingleTimeCommands() {
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = commandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	return commandBuffer;
}

void VulkanToolkit::endSingleTimeCommands(VkCommandBuffer commandBuffer) {
	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(mainQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(mainQueue);

	vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
}

void VulkanToolkit::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
	VkCommandBuffer commandBuffer = beginSingleTimeCommands();

	VkBufferCopy copyRegion = {};
	copyRegion.size = size;
	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

	endSingleTimeCommands(commandBuffer);
}

void VulkanToolkit::transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout) {
	VkCommandBuffer commandBuffer = beginSingleTimeCommands();

	VkImageMemoryBarrier barrier = {};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = image;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;

	if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	}
	else {
		throw std::invalid_argument("unsupported layout transition!");
	}

	vkCmdPipelineBarrier(
		commandBuffer,
		VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
		0,
		0, nullptr,
		0, nullptr,
		1, &barrier
	);

	endSingleTimeCommands(commandBuffer);
}


void VulkanToolkit::copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height) {
	VkCommandBuffer commandBuffer = beginSingleTimeCommands();

	VkBufferImageCopy region = {};
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;
	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;
	region.imageOffset = { 0, 0, 0 };
	region.imageExtent = {
		width,
		height,
		1
	};

	vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

	endSingleTimeCommands(commandBuffer);
}