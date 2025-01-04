#define VMA_IMPLEMENTATION
#include "SEVulkan.h"
#include "SEDDSLoader.h"

//DEBUG 
//-------------------------------------------------------------------------------------------------------------------------------------------
static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData) 
{
	OutputDebugStringA(pCallbackData->pMessage);

	return VK_FALSE;
}

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, 
	const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, 
	const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) 
{
	PFN_vkCreateDebugUtilsMessengerEXT func = 
		(PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr) 
	{
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	}
	else 
	{
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, 
	VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) 
{
	PFN_vkDestroyDebugUtilsMessengerEXT func = 
		(PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr) 
	{
		func(instance, debugMessenger, pAllocator);
	}
}

void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT* debugCreateInfo)
{
	debugCreateInfo->sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	debugCreateInfo->pNext = nullptr;
	debugCreateInfo->flags = 0;
	debugCreateInfo->messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	debugCreateInfo->messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	debugCreateInfo->pfnUserCallback = DebugCallback;
	debugCreateInfo->pUserData = nullptr;
}

void SetupDebugMessenger(SEVulkan* vulk)
{
	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
	PopulateDebugMessengerCreateInfo(&debugCreateInfo);

	VULKAN_ERROR_CHECK(CreateDebugUtilsMessengerEXT(vulk->instance, &debugCreateInfo, nullptr, &vulk->debugMessenger));
}

//-------------------------------------------------------------------------------------------------------------------------------------------

//SUPPORT CHECK
//-------------------------------------------------------------------------------------------------------------------------------------------
bool CheckValidationLayerSupport(const char** layerNames, uint32_t numLayers)
{
	uint32_t layerCount = 0;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	VkLayerProperties* layers = (VkLayerProperties*)calloc(layerCount, sizeof(VkLayerProperties));
	vkEnumerateInstanceLayerProperties(&layerCount, layers);

	for (uint32_t i = 0; i < numLayers; ++i)
	{
		bool layerFound = false;
		for (uint32_t j = 0; j < layerCount; ++j)
		{
			if (strcmp(layerNames[i], layers[j].layerName) == 0)
			{
				layerFound = true;
				break;
			}
		}

		if (!layerFound)
		{
			free(layers);
			return false;
		}
	}

	free(layers);
	return true;
}

bool CheckDeviceExtensionSupport(const SEVulkan* vulk, const char** extensionNames, uint32_t numExtensions)
{
	uint32_t extensionCount = 0;
	vkEnumerateDeviceExtensionProperties(vulk->physicalDevice, nullptr, &extensionCount, nullptr);

	VkExtensionProperties* extensions = (VkExtensionProperties*)calloc(extensionCount, sizeof(VkExtensionProperties));
	vkEnumerateDeviceExtensionProperties(vulk->physicalDevice, nullptr, &extensionCount, extensions);

	for (uint32_t i = 0; i < numExtensions; ++i)
	{
		bool extensionFound = false;
		for (uint32_t j = 0; j < extensionCount; ++j)
		{
			if (strcmp(extensionNames[i], extensions[j].extensionName) == 0)
			{
				extensionFound = true;
				break;
			}
		}

		if (!extensionFound)
		{
			free(extensions);
			return false;
		}
	}

	free(extensions);
	return true;
}
//-------------------------------------------------------------------------------------------------------------------------------------------

//INIT AND DESTROY
//-------------------------------------------------------------------------------------------------------------------------------------------
void CreateInstance(SEVulkan* vulk, const char* appName)
{
	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pNext = nullptr;
	appInfo.pApplicationName = appName;
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "Second Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_3;

	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

	VkExtensionProperties* extensions = (VkExtensionProperties*)calloc(extensionCount, sizeof(VkExtensionProperties));
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions);

	char** extNames = (char**)calloc(extensionCount, sizeof(char**));

	for (uint32_t i = 0; i < extensionCount; ++i)
	{
		extNames[i] = extensions[i].extensionName;
	}

	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.flags = 0;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledExtensionCount = extensionCount;
	createInfo.ppEnabledExtensionNames = extNames;

	if (vulk->enableValidationLayers)
	{
		const char* layer[] = { "VK_LAYER_KHRONOS_validation" };
		if (!CheckValidationLayerSupport(layer, 1))
		{
			MessageBox(nullptr, L"Validation Layers Requested, But Not Available! Exiting Program.", L"Validation Error", MB_OK);
			free(extNames);
			exit(2);
		}

		createInfo.enabledLayerCount = 1;
		createInfo.ppEnabledLayerNames = layer;

		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
		PopulateDebugMessengerCreateInfo(&debugCreateInfo);
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
	}
	else
	{
		createInfo.pNext = nullptr;
		createInfo.enabledLayerCount = 0;
		createInfo.ppEnabledLayerNames = nullptr;
	}

	VULKAN_ERROR_CHECK(vkCreateInstance(&createInfo, nullptr, &vulk->instance));

	free(extNames);
	free(extensions);
}

void CreatePhysicalDevice(SEVulkan* vulk)
{
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(vulk->instance, &deviceCount, nullptr);
	if (deviceCount == 0)
	{
		MessageBox(nullptr, L"Failed to find any GPUs with Vulkan Support. Exiting Program.", L"No Vulkan Supported GPU Found", MB_OK);
		exit(2);
	}

	VkPhysicalDevice* device = (VkPhysicalDevice*)calloc(deviceCount, sizeof(VkPhysicalDevice));
	vkEnumeratePhysicalDevices(vulk->instance, &deviceCount, device);

	for (uint32_t i = 0; i < deviceCount; ++i)
	{
		VkPhysicalDeviceProperties deviceProperties;
		VkPhysicalDeviceFeatures deviceFeatures;
		vkGetPhysicalDeviceProperties(device[i], &deviceProperties);
		vkGetPhysicalDeviceFeatures(device[i], &deviceFeatures);

		if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
		{
			vulk->physicalDevice = device[i];
			return;
		}
	}

	MessageBox(nullptr, L"Couldn't find a discrete gpu. Exiting Program.", L"No discrete GPU found.", MB_OK);
	exit(2);
}

#define GRAPHICS_FAM_INDEX 0
#define TRANSFER_FAM_INDEX 1
#define COMPUTE_FAM_INDEX 2

void FindQueueFamilies(SEVulkan* vulk)
{
	bool graphicsFam = false;
	bool computeFam = false;
	bool transferFam = false;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(vulk->physicalDevice, &queueFamilyCount, nullptr);
	
	VkQueueFamilyProperties* queueFamilies = (VkQueueFamilyProperties*)calloc(queueFamilyCount, sizeof(VkQueueFamilyProperties));
	vkGetPhysicalDeviceQueueFamilyProperties(vulk->physicalDevice, &queueFamilyCount, queueFamilies);

	for (uint32_t i = 0; i < queueFamilyCount; ++i)
	{
		std::string qFam = "Index i = " + std::to_string(i) + " the queue family flags = "
			+ std::to_string(queueFamilies[i].queueFlags) + "\n";
		OutputDebugStringA(qFam.c_str());

		//Looking for a queue family that supports graphics
		if ((queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT))
		{
			qFam = "Graphics queue family index = " + std::to_string(i) + "\n";
			OutputDebugStringA(qFam.c_str());
			graphicsFam = true;
			vulk->familyIndices[GRAPHICS_FAM_INDEX] = i;
		}

		//Looking for a queue family that supports compute only (a dedicated compute queue)
		else if ((queueFamilies[i].queueFlags & VK_QUEUE_COMPUTE_BIT) && 
			(queueFamilies[i].queueFlags & ~VK_QUEUE_GRAPHICS_BIT))
		{
			qFam = "Compute queue family index = " + std::to_string(i) + "\n";
			OutputDebugStringA(qFam.c_str());
			vulk->familyIndices[COMPUTE_FAM_INDEX] = i;
			computeFam = true;
		}

		//Looking for a queue family that supports transfer only
		else if ((queueFamilies[i].queueFlags & VK_QUEUE_TRANSFER_BIT) &&
			(queueFamilies[i].queueFlags & ~VK_QUEUE_GRAPHICS_BIT) && 
			(queueFamilies[i].queueFlags & ~VK_QUEUE_COMPUTE_BIT))
		{
			qFam = "Transfer queue family index = " + std::to_string(i) + "\n";
			OutputDebugStringA(qFam.c_str());
			vulk->familyIndices[TRANSFER_FAM_INDEX] = i;
			transferFam = true;
		}
	}

	if (!(graphicsFam && computeFam && transferFam))
	{
		MessageBox(nullptr, L"Not all queue families are supported. Exiting Program.", L"All necessary queue families not found.", MB_OK);
		free(queueFamilies);
		exit(2);
	}

	free(queueFamilies);
}

void CreateLogicalDevice(SEVulkan* vulk)
{
	VkDeviceQueueCreateInfo* queueCreateInfos = (VkDeviceQueueCreateInfo*)calloc(3, sizeof(VkDeviceQueueCreateInfo));

	float queuePrio = 1.0f;

	//Graphics queue
	queueCreateInfos[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfos[0].pNext = nullptr;
	//queueCreateInfos[0].flags = 0;
	queueCreateInfos[0].queueFamilyIndex = vulk->familyIndices[GRAPHICS_FAM_INDEX];
	queueCreateInfos[0].queueCount = 1;
	queueCreateInfos[0].pQueuePriorities = &queuePrio;

	//Dedicated compute queue
	queueCreateInfos[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfos[1].pNext = nullptr;
	//queueCreateInfos[1].flags = 0;
	queueCreateInfos[1].queueFamilyIndex = vulk->familyIndices[COMPUTE_FAM_INDEX];
	queueCreateInfos[1].queueCount = 1;
	queueCreateInfos[1].pQueuePriorities = &queuePrio;

	//Dedicated transfer queue
	queueCreateInfos[2].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfos[2].pNext = nullptr;
	//queueCreateInfos[2].flags = 0;
	queueCreateInfos[2].queueFamilyIndex = vulk->familyIndices[TRANSFER_FAM_INDEX];
	queueCreateInfos[2].queueCount = 1;
	queueCreateInfos[2].pQueuePriorities = &queuePrio;

	//NEED TO DO LATER
	VkPhysicalDeviceFeatures deviceFeatures{};

	const char* extensions[] = { "VK_KHR_swapchain" };
	if (!CheckDeviceExtensionSupport(vulk, extensions, 1))
	{
		MessageBox(nullptr, L"No swapchain support. Exiting Program.", L"Swapchain error.", MB_OK);
		free(queueCreateInfos);
		exit(2);
	}

	VkDeviceCreateInfo deviceCreateInfo{};
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.pNext = nullptr;
	deviceCreateInfo.queueCreateInfoCount = 3;
	deviceCreateInfo.pQueueCreateInfos = queueCreateInfos;
	deviceCreateInfo.enabledExtensionCount = 1;
	deviceCreateInfo.ppEnabledExtensionNames = extensions;
	deviceCreateInfo.pEnabledFeatures = &deviceFeatures;

	VULKAN_ERROR_CHECK(vkCreateDevice(vulk->physicalDevice, &deviceCreateInfo, nullptr, &vulk->logicalDevice));

	free(queueCreateInfos);
}

void CreateSurface(SEVulkan* vulk, SEWindow* window)
{
	VkWin32SurfaceCreateInfoKHR surfaceInfo{};
	surfaceInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	surfaceInfo.pNext = nullptr;
	surfaceInfo.flags = 0;
	surfaceInfo.hinstance = GetModuleHandle(nullptr);
	surfaceInfo.hwnd = window->wndHandle;

	VULKAN_ERROR_CHECK(vkCreateWin32SurfaceKHR(vulk->instance, &surfaceInfo, nullptr, &vulk->surface));
}

void InitVma(SEVulkan* vulk)
{

	VmaAllocatorCreateInfo allocatorCreateInfo = {};
	allocatorCreateInfo.flags = 0;
	allocatorCreateInfo.vulkanApiVersion = VK_API_VERSION_1_3;
	allocatorCreateInfo.physicalDevice = vulk->physicalDevice;
	allocatorCreateInfo.device = vulk->logicalDevice;
	allocatorCreateInfo.instance = vulk->instance;

	vmaCreateAllocator(&allocatorCreateInfo, &vulk->allocator);
}

void DestroyVma(SEVulkan* vulk)
{
	vmaDestroyAllocator(vulk->allocator);
}

struct VulkanCopyEngine
{
	VkQueue transferQueue;
	VkCommandPool commandPool;
	VkCommandBuffer commandBuffer;
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	VmaAllocation stagingBufferAllocation;
}gVulkanCopyEngine;

void InitCopyEngine(SEVulkan* vulk)
{
	vkGetDeviceQueue(vulk->logicalDevice, vulk->familyIndices[TRANSFER_FAM_INDEX], 0, &gVulkanCopyEngine.transferQueue);

	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.pNext = nullptr;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = vulk->familyIndices[TRANSFER_FAM_INDEX];

	VULKAN_ERROR_CHECK(vkCreateCommandPool(vulk->logicalDevice, &poolInfo, nullptr, &gVulkanCopyEngine.commandPool));

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.pNext = nullptr;
	allocInfo.commandPool = gVulkanCopyEngine.commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 1;

	VULKAN_ERROR_CHECK(vkAllocateCommandBuffers(vulk->logicalDevice, &allocInfo, &gVulkanCopyEngine.commandBuffer));
}

void DestroyCopyEngine(SEVulkan* vulk)
{
	vkDestroyCommandPool(vulk->logicalDevice, gVulkanCopyEngine.commandPool, nullptr);
}

#define MAX_NUM_DESCRIPTORS 16
VkDescriptorPool gDescriptorPool;
void CreateVulkanDescriptorPool(SEVulkan* vulk)
{
	VkDescriptorPoolSize poolSize[2]{};

	poolSize[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSize[0].descriptorCount = MAX_NUM_DESCRIPTORS;

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.pNext = nullptr;
	poolInfo.flags = 0;
	poolInfo.maxSets = MAX_NUM_DESCRIPTORS;
	poolInfo.poolSizeCount = 1;
	poolInfo.pPoolSizes = poolSize;

	VULKAN_ERROR_CHECK(vkCreateDescriptorPool(vulk->logicalDevice, &poolInfo, nullptr, &gDescriptorPool));
}

void DestroyVulkanDescriptorPool(SEVulkan* vulk)
{
	vkDestroyDescriptorPool(vulk->logicalDevice, gDescriptorPool, nullptr);
}

void InitVulkan(SEVulkan* vulk, SEWindow* window, const char* appName)
{
	CreateInstance(vulk, appName);

	if (vulk->enableValidationLayers)
	{
		SetupDebugMessenger(vulk);
	}

	CreatePhysicalDevice(vulk);

	CreateSurface(vulk, window);

	FindQueueFamilies(vulk);

	CreateLogicalDevice(vulk);

	InitVma(vulk);

	InitCopyEngine(vulk);

	CreateVulkanDescriptorPool(vulk);
}

void DestroyVulkan(SEVulkan* vulk)
{
	DestroyVulkanDescriptorPool(vulk);

	DestroyCopyEngine(vulk);

	DestroyVma(vulk);

	vkDestroyDevice(vulk->logicalDevice, nullptr);

	if (vulk->enableValidationLayers)
	{
		DestroyDebugUtilsMessengerEXT(vulk->instance, vulk->debugMessenger, nullptr);
	}

	vkDestroySurfaceKHR(vulk->instance, vulk->surface, nullptr);
	vkDestroyInstance(vulk->instance, nullptr);
}
//-------------------------------------------------------------------------------------------------------------------------------------------

void GetQueueHandle(const SEVulkan* vulk, const SEVulkanQueueInfo* queueInfo, SEVulkanQueue* queue)
{
	switch (queueInfo->queueType)
	{
	case SE_GRAPHICS_QUEUE:
		vkGetDeviceQueue(vulk->logicalDevice, vulk->familyIndices[GRAPHICS_FAM_INDEX], queueInfo->index, &queue->queue);
		return;

	case SE_COMPUTE_QUEUE:
		vkGetDeviceQueue(vulk->logicalDevice, vulk->familyIndices[COMPUTE_FAM_INDEX], queueInfo->index, &queue->queue);
		return;
	}
}

//SWAPCHAIN
//-------------------------------------------------------------------------------------------------------------------------------------------
struct SwapChainInfo
{
	VkSurfaceCapabilitiesKHR capabilities;

	uint32_t formatCount;
	VkSurfaceFormatKHR* formats;

	uint32_t presentModeCount;
	VkPresentModeKHR* presentModes;
};

void FreeSwapChainInfo(SwapChainInfo* sci)
{
	free(sci->formats);
	free(sci->presentModes);
}

void QuerySwapChainSupport(const SEVulkan* vulk, SwapChainInfo* sci)
{
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vulk->physicalDevice, vulk->surface, &sci->capabilities);

	vkGetPhysicalDeviceSurfaceFormatsKHR(vulk->physicalDevice, vulk->surface, &sci->formatCount, nullptr);

	if (sci->formatCount == 0)
	{
		MessageBox(nullptr, L"No surface formats are supported by the GPU. Exiting Progamr.", L"Surface formats error.", MB_OK);
		exit(2);
	}
	sci->formats = (VkSurfaceFormatKHR*)calloc(sci->formatCount, sizeof(VkSurfaceFormatKHR));
	vkGetPhysicalDeviceSurfaceFormatsKHR(vulk->physicalDevice, vulk->surface, &sci->formatCount, sci->formats);

	vkGetPhysicalDeviceSurfacePresentModesKHR(vulk->physicalDevice, vulk->surface, &sci->presentModeCount, nullptr);
	if (sci->presentModeCount == 0)
	{
		MessageBox(nullptr, L"No present modes are supported by the GPU. Exiting Progamr.", L"Present modes error.", MB_OK);
		free(sci->formats);
		exit(2);
	}
	sci->presentModes = (VkPresentModeKHR*)calloc(sci->presentModeCount, sizeof(VkPresentModeKHR));
	vkGetPhysicalDeviceSurfacePresentModesKHR(vulk->physicalDevice, vulk->surface, &sci->presentModeCount, sci->presentModes);
}

VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const SwapChainInfo* sci)
{
	for (uint32_t i = 0; i < sci->formatCount; ++i)
	{
		if (sci->formats[i].format == VK_FORMAT_R8G8B8A8_SRGB && sci->formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			return sci->formats[i];
	}

	return sci->formats[0];
}

VkPresentModeKHR ChooseSwapPresentMode(const SwapChainInfo* sci)
{
	for (uint32_t i = 0; i < sci->presentModeCount; ++i)
	{
		if (sci->presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
			return sci->presentModes[i];
	}

	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D ChooseSwapExtent(const SwapChainInfo* sci, const SEWindow* window)
{
	VkExtent2D extent{};

	RECT rect{};
	GetClientRect(window->wndHandle, &rect);
	extent.width = rect.right - rect.left;
	extent.height = rect.bottom - rect.top;

	if (extent.width < sci->capabilities.minImageExtent.width)
	{
		extent.width = sci->capabilities.minImageExtent.width;
	}
	else if (extent.width > sci->capabilities.maxImageExtent.width)
	{
		extent.width = sci->capabilities.maxImageExtent.width;
	}

	if (extent.height < sci->capabilities.minImageExtent.height)
	{
		extent.height = sci->capabilities.minImageExtent.height;
	}
	else if (extent.height > sci->capabilities.maxImageExtent.height)
	{
		extent.height = sci->capabilities.maxImageExtent.height;
	}

	return extent;
}

void CreateVulkanSwapChain(SEVulkan* vulk, SEWindow* window, SEVulkanSwapChain* swapChain)
{
	SwapChainInfo sci{};
	QuerySwapChainSupport(vulk, &sci);

	VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(&sci);
	VkPresentModeKHR presentMode = ChooseSwapPresentMode(&sci);
	VkExtent2D extent = ChooseSwapExtent(&sci, window);

	std::string size = "Swapchain size: width = " + std::to_string(extent.width) + ", height = " + std::to_string(extent.height) + "\n";
	OutputDebugStringA(size.c_str());

	uint32_t imageCount = sci.capabilities.minImageCount + 1;
	if (sci.capabilities.maxImageCount > 0 && imageCount > sci.capabilities.maxImageCount)
	{
		imageCount = sci.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR swapCreateInfo{};
	swapCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapCreateInfo.pNext = nullptr;
	swapCreateInfo.flags = 0;
	swapCreateInfo.surface = vulk->surface;
	swapCreateInfo.minImageCount = imageCount;
	swapCreateInfo.imageFormat = surfaceFormat.format;
	swapCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
	swapCreateInfo.imageExtent = extent;
	swapCreateInfo.imageArrayLayers = 1;
	swapCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	swapCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	swapCreateInfo.queueFamilyIndexCount = 0;
	swapCreateInfo.pQueueFamilyIndices = 0;
	swapCreateInfo.preTransform = sci.capabilities.currentTransform;
	swapCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	swapCreateInfo.presentMode = presentMode;
	swapCreateInfo.clipped = VK_TRUE;
	swapCreateInfo.oldSwapchain = VK_NULL_HANDLE;

	VULKAN_ERROR_CHECK(vkCreateSwapchainKHR(vulk->logicalDevice, &swapCreateInfo, nullptr, &swapChain->swapChain));

	vkGetSwapchainImagesKHR(vulk->logicalDevice, swapChain->swapChain, &swapChain->imageCount, nullptr);
	swapChain->images = (VkImage*)calloc(swapChain->imageCount, sizeof(VkImage));
	vkGetSwapchainImagesKHR(vulk->logicalDevice, swapChain->swapChain, &swapChain->imageCount, swapChain->images);

	swapChain->format = surfaceFormat.format;
	swapChain->extent = extent;

	FreeSwapChainInfo(&sci);

	swapChain->imageViews = (SEVulkanImageView*)calloc(swapChain->imageCount, sizeof(SEVulkanImageView));

	for (uint32_t i = 0; i < swapChain->imageCount; ++i)
	{
		SEImageViewInfo info{};
		info.image = swapChain->images[i];
		info.viewType = VK_IMAGE_VIEW_TYPE_2D;
		info.format = TinyImageFormat_FromVkFormat((TinyImageFormat_VkFormat)swapChain->format);
		info.firstMipLevel = 0;
		info.numMipLevels = 1;
		info.firstArrayLayer = 0;
		info.numLayers = 1;
		CreateImageView(vulk, &info, &swapChain->imageViews[i]);
	}

	swapChain->frameBuffers = (VkFramebuffer*)calloc(swapChain->imageCount, sizeof(VkFramebuffer));
}

void DestroyVulkanSwapChain(SEVulkan* vulk, SEVulkanSwapChain* swapChain)
{

	for (uint32_t i = 0; i < swapChain->imageCount; ++i)
	{
		DestroyVulkanFrameBuffer(vulk, &swapChain->frameBuffers[i]);
		vkDestroyImageView(vulk->logicalDevice, swapChain->imageViews[i].imageView, nullptr);
	}
	free(swapChain->frameBuffers);
	free(swapChain->imageViews);
	free(swapChain->images);
	vkDestroySwapchainKHR(vulk->logicalDevice, swapChain->swapChain, nullptr);
}

void CreateImageView(SEVulkan* vulk, SEImageViewInfo* info, SEVulkanImageView* imageView)
{
	VkImageViewCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.flags = 0;
	createInfo.image = info->image;
	createInfo.viewType = info->viewType;
	createInfo.format = (VkFormat)TinyImageFormat_ToVkFormat(info->format);
	createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	createInfo.subresourceRange.baseMipLevel = info->firstMipLevel;
	createInfo.subresourceRange.levelCount = info->numMipLevels;
	createInfo.subresourceRange.baseArrayLayer = info->firstArrayLayer;
	createInfo.subresourceRange.layerCount = info->numLayers;

	VULKAN_ERROR_CHECK(vkCreateImageView(vulk->logicalDevice, &createInfo, nullptr, &imageView->imageView));
}

void DestroyImageView(SEVulkan* vulk, SEVulkanImageView* imageView)
{
	vkDestroyImageView(vulk->logicalDevice, imageView->imageView, nullptr);
}
//-------------------------------------------------------------------------------------------------------------------------------------------

//SHADERS
//-------------------------------------------------------------------------------------------------------------------------------------------
void CreateVulkanShader(SEVulkan* vulk, const char* filename, SEShaderType type, SEVulkanShader* shader)
{
	char* buffer = nullptr;
	uint32_t fileSize = 0;
	ReadFile(filename, &buffer, &fileSize, BINARY);
	uint32_t num = 3;
	uint32_t newSize = (fileSize + num) & ~num; //make filesize the nearest number divisible by 4
	char* code = (char*)calloc(newSize, sizeof(char));
	memcpy(code, buffer, fileSize);

	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.flags = 0;
	createInfo.codeSize = newSize;
	createInfo.pCode = (uint32_t*)code;

	VULKAN_ERROR_CHECK(vkCreateShaderModule(vulk->logicalDevice, &createInfo, nullptr, &shader->shaderModule));

	VkPipelineShaderStageCreateInfo stageCreateInfo{};
	stageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	stageCreateInfo.pNext = nullptr;
	stageCreateInfo.flags = 0;
	switch (type)
	{
	case SE_VERTEX_SHADER:
		stageCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		break;

	case SE_PIXEL_SHADER:
		stageCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		break;

	case SE_COMPUTE_SHADER:
		stageCreateInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
		break;
	}
	stageCreateInfo.module = shader->shaderModule;
	stageCreateInfo.pName = "main";
	stageCreateInfo.pSpecializationInfo = nullptr;

	shader->shaderCreateInfo = stageCreateInfo;

	FreeFileBuffer(buffer);
	free(code);
}

void DestroyVulkanShader(SEVulkan* vulk, SEVulkanShader* shader)
{
	vkDestroyShaderModule(vulk->logicalDevice, shader->shaderModule, nullptr);
}
//-------------------------------------------------------------------------------------------------------------------------------------------

//PIPELINE
//-------------------------------------------------------------------------------------------------------------------------------------------
void CreateInputAssemblyInfo(SEVulkanPipleineInfo* info, VkPipelineInputAssemblyStateCreateInfo* inputAssemblyInfo)
{
	inputAssemblyInfo->sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssemblyInfo->pNext = nullptr;
	inputAssemblyInfo->flags = 0;

	switch (info->topology)
	{
	case SE_POINT_LIST:
		inputAssemblyInfo->topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
		break;

	case SE_LINE_LIST:
		inputAssemblyInfo->topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
		break;

	case SE_LINE_STRIP:
		inputAssemblyInfo->topology = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
		break;

	case SE_TRIANGLE_LIST:
		inputAssemblyInfo->topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		break;

	case SE_TRIANGLE_STRIP:
		inputAssemblyInfo->topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
		break;
	}

	inputAssemblyInfo->primitiveRestartEnable = VK_FALSE;
}

void CreateRasterizerInfo(SEVulkanPipleineInfo* info, VkPipelineRasterizationStateCreateInfo* rasInfo)
{
	rasInfo->sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasInfo->pNext = nullptr;
	rasInfo->flags = 0;
	rasInfo->depthClampEnable = VK_FALSE;
	rasInfo->rasterizerDiscardEnable = VK_FALSE;
	rasInfo->polygonMode = VK_POLYGON_MODE_FILL;

	switch (info->rasInfo.cullMode)
	{
	case SE_CULL_NONE:
		rasInfo->cullMode = VK_CULL_MODE_NONE;
		break;

	case SE_CULL_FRONT:
		rasInfo->cullMode = VK_CULL_MODE_FRONT_BIT;
		break;

	case SE_CULL_BACK:
		rasInfo->cullMode = VK_CULL_MODE_BACK_BIT;
		break;
	}

	switch (info->rasInfo.faceMode)
	{
	case SE_FACE_CLOCKWISE:
		rasInfo->frontFace = VK_FRONT_FACE_CLOCKWISE;
		break;

	case SE_FACE_COUNTER_CLOCKWISE:
		rasInfo->frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		break;
	}

	rasInfo->depthBiasEnable = VK_FALSE;
	rasInfo->depthBiasConstantFactor = 0.0f;
	rasInfo->depthBiasClamp = 0.0f;
	rasInfo->depthBiasSlopeFactor = 0.0f;
	rasInfo->lineWidth = info->rasInfo.lineWidth;
}

void CreateRenderPass(SEVulkan* vulk, SEVulkanPipeline* pipeline, VkFormat format)
{
	VkAttachmentDescription colorAttachment{};
	colorAttachment.flags = 0;
	colorAttachment.format = format;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentRef{};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass{};
	subpass.flags = 0;
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;

	VkSubpassDependency dependency{};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	dependency.dependencyFlags = 0;

	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.pNext = nullptr;
	renderPassInfo.flags = 0;
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = &colorAttachment;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;

	VULKAN_ERROR_CHECK(vkCreateRenderPass(vulk->logicalDevice, &renderPassInfo, nullptr, &pipeline->renderPass));
}

void CreateVulkanBindingDescription(SEVulkanVertexBindingInfo* bindingInfo, VkVertexInputBindingDescription* desc)
{
	desc->binding = bindingInfo->binding;
	desc->stride = bindingInfo->stride;

	switch (bindingInfo->inputRate)
	{
	case SE_PER_VERTEX:
		desc->inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		break;

	case SE_PER_INSTANCE:
		desc->inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;
		break;
	}
}

void CreateVulkanAttributeDescription(SEVulkanAttributeInfo* attribInfo, VkVertexInputAttributeDescription* desc, uint32_t numInfos)
{
	for (uint32_t i = 0; i < numInfos; ++i)
	{
		VkVertexInputAttributeDescription attribDesc{};
		attribDesc.binding = attribInfo[i].binding;
		attribDesc.location = attribInfo[i].location;
		attribDesc.format = (VkFormat)TinyImageFormat_ToVkFormat(attribInfo[i].format);
		attribDesc.offset = attribInfo[i].offset;

		desc[i] = attribDesc;
	}
}

void CreateVertexDescriptions(SEVulkanPipleineInfo* pipelineInfo, 
	VkVertexInputBindingDescription* bindingDesc, VkVertexInputAttributeDescription* attribDesc,
	VkPipelineVertexInputStateCreateInfo* createInfo)
{
	CreateVulkanBindingDescription(&pipelineInfo->bindingInfo, bindingDesc);

	CreateVulkanAttributeDescription(pipelineInfo->attributeInfo, attribDesc, pipelineInfo->numAttributes);

	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	createInfo->sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	createInfo->vertexBindingDescriptionCount = 1;
	createInfo->pVertexBindingDescriptions = bindingDesc;
	createInfo->vertexAttributeDescriptionCount = pipelineInfo->numAttributes;
	createInfo->pVertexAttributeDescriptions = attribDesc;
}

void CreateVulkanDescriptorSetLayout(SEVulkan* vulk, SEVulkanDescritporSetLayoutInfo* info, SEVulkanDescriptorSetLayout* dsl)
{
	VkDescriptorSetLayoutBinding layoutBinding[MAX_NUM_DESCRIPTOR_SET_LAYOUT_BINDINGS]{};
	for (uint32_t i = 0; i < info->numBindings; ++i)
	{
		layoutBinding[i].binding = info->bindingInfo[i].binding;

		switch (info->bindingInfo[i].type)
		{
		case SE_DESCRITPTOR_UNIFORM_BUFFER:
			layoutBinding[i].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			break;
		}

		layoutBinding[i].descriptorCount = info->bindingInfo[i].numDescriptors;

		if (info->bindingInfo[i].stages & SE_VERTEX_STAGE)
		{
			layoutBinding[i].stageFlags |= VK_SHADER_STAGE_VERTEX_BIT;
		}

		if (info->bindingInfo[i].stages & SE_PIXEL_STAGE)
		{
			layoutBinding[i].stageFlags |= VK_SHADER_STAGE_FRAGMENT_BIT;
		}

		layoutBinding[i].pImmutableSamplers = nullptr;
	}

	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.pNext = nullptr;
	layoutInfo.flags = 0;
	layoutInfo.bindingCount = info->numBindings;
	layoutInfo.pBindings = layoutBinding;

	VULKAN_ERROR_CHECK(vkCreateDescriptorSetLayout(vulk->logicalDevice, &layoutInfo, nullptr, &dsl->descriptorSetLayout))
}

void CreatePipelineLayout(SEVulkan* vulk, SEVulkanPipleineInfo* info, SEVulkanPipeline* pipeline)
{
	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = &info->descriptorSetLayout->descriptorSetLayout;
	pipelineLayoutInfo.pushConstantRangeCount = 0;
	pipelineLayoutInfo.pPushConstantRanges = nullptr;

	VULKAN_ERROR_CHECK(vkCreatePipelineLayout(vulk->logicalDevice, &pipelineLayoutInfo, nullptr, &pipeline->pipelineLayout));
}

void DestroyVulkanDescriptorSetLayout(SEVulkan* vulk, SEVulkanDescriptorSetLayout* dsl)
{
	vkDestroyDescriptorSetLayout(vulk->logicalDevice, dsl->descriptorSetLayout, nullptr);
}

void CreateVulkanPipeline(SEVulkan* vulk, SEVulkanPipleineInfo* info, SEVulkanPipeline* pipeline)
{
	VkVertexInputBindingDescription bindingDesc{};
	VkVertexInputAttributeDescription attribDesc[NUM_ATTRIBUTES]{};
	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	CreateVertexDescriptions(info, &bindingDesc, attribDesc, &vertexInputInfo);

	VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};
	CreateInputAssemblyInfo(info, &inputAssemblyInfo);

	VkDynamicState dynamicStates[2] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
	VkPipelineDynamicStateCreateInfo dynamicStateInfo{};
	dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicStateInfo.pNext = nullptr;
	dynamicStateInfo.flags = 0;
	dynamicStateInfo.dynamicStateCount = 2;
	dynamicStateInfo.pDynamicStates = dynamicStates;

	VkPipelineViewportStateCreateInfo viewportState{};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.scissorCount = 1;

	VkPipelineRasterizationStateCreateInfo rasInfo{};
	CreateRasterizerInfo(info, &rasInfo);

	VkPipelineMultisampleStateCreateInfo multisampling{};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampling.minSampleShading = 1.0f;
	multisampling.pSampleMask = nullptr;
	multisampling.alphaToCoverageEnable = VK_FALSE;
	multisampling.alphaToOneEnable = VK_FALSE;

	VkPipelineColorBlendAttachmentState colorBlendAttachment{};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;

	VkPipelineColorBlendStateCreateInfo colorBlending{};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f;
	colorBlending.blendConstants[1] = 0.0f;
	colorBlending.blendConstants[2] = 0.0f;
	colorBlending.blendConstants[3] = 0.0f;

	CreatePipelineLayout(vulk, info, pipeline);
	CreateRenderPass(vulk, pipeline, info->swapChainFormat);

	VkPipelineShaderStageCreateInfo shaders[] = { info->shaders[0].shaderCreateInfo, info->shaders[1].shaderCreateInfo };

	VkGraphicsPipelineCreateInfo pipelineInfo{};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.pNext = nullptr;
	pipelineInfo.flags = 0;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaders;
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssemblyInfo;
	pipelineInfo.pTessellationState = nullptr;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasInfo;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pDepthStencilState = nullptr;
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.pDynamicState = &dynamicStateInfo;
	pipelineInfo.layout = pipeline->pipelineLayout;
	pipelineInfo.renderPass = pipeline->renderPass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; 
	pipelineInfo.basePipelineIndex = -1;

	VULKAN_ERROR_CHECK(vkCreateGraphicsPipelines(vulk->logicalDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline->pipeline));
}

void DestroyVulkanPipeline(SEVulkan* vulk, SEVulkanPipeline* pipeline)
{
	vkDestroyPipeline(vulk->logicalDevice, pipeline->pipeline, nullptr);
	vkDestroyPipelineLayout(vulk->logicalDevice, pipeline->pipelineLayout, nullptr);
	vkDestroyRenderPass(vulk->logicalDevice, pipeline->renderPass, nullptr);
}

void CreateVulkanFrameBuffer(SEVulkan* vulk, SEVulkanSwapChain* swapChain, VkRenderPass* renderPass)
{
	for (uint32_t i = 0; i < swapChain->imageCount; ++i)
	{
		VkFramebufferCreateInfo frameBufferCreateInfo{};
		frameBufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		frameBufferCreateInfo.pNext = nullptr;
		frameBufferCreateInfo.flags = 0;
		frameBufferCreateInfo.renderPass = *renderPass;
		frameBufferCreateInfo.attachmentCount = 1;
		frameBufferCreateInfo.pAttachments = &swapChain->imageViews[i].imageView;
		frameBufferCreateInfo.width = swapChain->extent.width;
		frameBufferCreateInfo.height = swapChain->extent.height;
		frameBufferCreateInfo.layers = 1;

		VULKAN_ERROR_CHECK(vkCreateFramebuffer(vulk->logicalDevice, &frameBufferCreateInfo, nullptr, &swapChain->frameBuffers[i]));
	}
}

void DestroyVulkanFrameBuffer(SEVulkan* vulk, VkFramebuffer* frameBuffer)
{
	vkDestroyFramebuffer(vulk->logicalDevice, *frameBuffer, nullptr);
}
//-------------------------------------------------------------------------------------------------------------------------------------------

//COMMAND BUFFERS
//-------------------------------------------------------------------------------------------------------------------------------------------
void CreateVulkanCommandPool(SEVulkan* vulk, SEVulkanCommandPool* commandPool, SEVulkanQueueType queueType)
{
	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.pNext = nullptr;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

	switch (queueType)
	{
	case SE_GRAPHICS_QUEUE:
		poolInfo.queueFamilyIndex = vulk->familyIndices[GRAPHICS_FAM_INDEX];
		break;

	case SE_COMPUTE_QUEUE:
		poolInfo.queueFamilyIndex = vulk->familyIndices[COMPUTE_FAM_INDEX];
		break;
	}

	VULKAN_ERROR_CHECK(vkCreateCommandPool(vulk->logicalDevice, &poolInfo, nullptr, &commandPool->commandPool));
}

void DestroyVulkanCommandPool(SEVulkan* vulk, SEVulkanCommandPool* commandPool)
{
	vkDestroyCommandPool(vulk->logicalDevice, commandPool->commandPool, nullptr);
}

void CreateVulkanCommandBuffer(SEVulkan* vulk, SEVulkanCommandPool* commandPool,
	SEVulkanCommandBuffer* commandBuffer, uint32_t numCommandBuffers)
{
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.pNext = nullptr;
	allocInfo.commandPool = commandPool->commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = numCommandBuffers;

	VULKAN_ERROR_CHECK(vkAllocateCommandBuffers(vulk->logicalDevice, &allocInfo, &commandBuffer->commandBuffer));
}

//-------------------------------------------------------------------------------------------------------------------------------------------

//DRAW
//--------------------------------------------------------------------------------------------------------------------------------------------

void VulkanResetCommandBuffer(SEVulkanCommandBuffer* commandBuffer)
{
	VULKAN_ERROR_CHECK(vkResetCommandBuffer(commandBuffer->commandBuffer, 0));
}

void VulkanBeginCommandBuffer(SEVulkan* vulk, SEVulkanCommandBuffer* commandBuffer, SEVulkanPipeline* pipeline,
	SEVulkanSwapChain* swapChain, uint32_t imageIndex)
{
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.pNext = nullptr;
	beginInfo.flags = 0;
	beginInfo.pInheritanceInfo = nullptr;

	VULKAN_ERROR_CHECK(vkBeginCommandBuffer(commandBuffer->commandBuffer, &beginInfo));

	VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.pNext = nullptr;
	renderPassInfo.renderPass = pipeline->renderPass;
	renderPassInfo.framebuffer = swapChain->frameBuffers[imageIndex];
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = swapChain->extent;
	renderPassInfo.clearValueCount = 1;
	renderPassInfo.pClearValues = &clearColor;

	vkCmdBeginRenderPass(commandBuffer->commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void VulkanBindPipeline(SEVulkanCommandBuffer* commandBuffer, SEVulkanPipeline* pipeline, SEPipelineType type)
{
	switch (type)
	{
	case SE_GRAPHICS_PIPELINE:
		vkCmdBindPipeline(commandBuffer->commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->pipeline);
		break;

	case SE_COMPUTE_PIPELINE:
		vkCmdBindPipeline(commandBuffer->commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline->pipeline);
		break;
	}
}

void VulkanSetViewport(SEVulkanCommandBuffer* commandBuffer, SEViewportInfo* viewportInfo)
{
	VkViewport viewport{};
	viewport.x = viewportInfo->x;
	viewport.y = viewportInfo->y;
	viewport.width = viewportInfo->width;
	viewport.height = viewportInfo->height;
	viewport.minDepth = viewportInfo->minDepth;
	viewport.maxDepth = viewportInfo->maxDepth;
	vkCmdSetViewport(commandBuffer->commandBuffer, 0, 1, &viewport);
}

void VulkanSetScissor(SEVulkanCommandBuffer* commandBuffer, SEScissorInfo* scissorInfo)
{
	VkRect2D scissor{};
	scissor.offset.x = scissorInfo->x;
	scissor.offset.y = scissorInfo->y;
	scissor.extent.width = scissorInfo->width;
	scissor.extent.height = scissorInfo->height;
	vkCmdSetScissor(commandBuffer->commandBuffer, 0, 1, &scissor);
}

void VulkanDraw(SEVulkanCommandBuffer* commandBuffer, uint32_t vertexCount, 
	uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance)
{
	vkCmdDraw(commandBuffer->commandBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
}

void VulkanDrawIndexed(SEVulkanCommandBuffer* commandBuffer, uint32_t indexCount,
	uint32_t instanceCount, uint32_t firstIndex, uint32_t vertexOffset, uint32_t firstInstance)
{
	vkCmdDrawIndexed(commandBuffer->commandBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}

void VulkanEndCommandBuffer(SEVulkanCommandBuffer* commandBuffer)
{
	vkCmdEndRenderPass(commandBuffer->commandBuffer);
	VULKAN_ERROR_CHECK(vkEndCommandBuffer(commandBuffer->commandBuffer));
}
//-------------------------------------------------------------------------------------------------------------------------------------------

//SYNCHRONIZATION
//-------------------------------------------------------------------------------------------------------------------------------------------
void CreateVulkanFence(SEVulkan* vulk, SEVulkanFence* fence)
{
	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.pNext = nullptr;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
	
	VULKAN_ERROR_CHECK(vkCreateFence(vulk->logicalDevice, &fenceInfo, nullptr, &fence->fence));
}

void DestroyVulkanFence(SEVulkan* vulk, SEVulkanFence* fence)
{
	vkDestroyFence(vulk->logicalDevice, fence->fence, nullptr);
}

void VulkanWaitForFence(SEVulkan* vulk, SEVulkanFence* fence)
{
	vkWaitForFences(vulk->logicalDevice, 1, &fence->fence, VK_TRUE, UINT64_MAX);
	vkResetFences(vulk->logicalDevice, 1, &fence->fence);
}

void CreateVulkanSemaphore(SEVulkan* vulk, SEVulkanSemaphore* semaphore)
{
	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	semaphoreInfo.pNext = nullptr;
	semaphoreInfo.flags = 0;

	VULKAN_ERROR_CHECK(vkCreateSemaphore(vulk->logicalDevice, &semaphoreInfo, nullptr, &semaphore->semaphore));
}

void DestroyVulkanSemaphore(SEVulkan* vulk, SEVulkanSemaphore* semaphore)
{
	vkDestroySemaphore(vulk->logicalDevice, semaphore->semaphore, nullptr);
}

//-------------------------------------------------------------------------------------------------------------------------------------------

void VulkanAcquireNextImage(SEVulkan* vulk, SEVulkanSwapChain* swapChain, SEVulkanSemaphore* imageAvailableSemaphore, uint32_t* imageIndex)
{
	vkAcquireNextImageKHR(vulk->logicalDevice, swapChain->swapChain, UINT64_MAX, imageAvailableSemaphore->semaphore,
		nullptr, imageIndex);
}

//SUBMIT AND PRESENT
//-------------------------------------------------------------------------------------------------------------------------------------------
void VulkanQueueSubmit(SEVulkan* vulk, VulkanQueueSubmitInfo* info)
{
	VkSemaphore waitSemaphores[] = { info->waitSemaphore->semaphore };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	VkSemaphore signalSemaphores[] = { info->signalSemaphore->semaphore };

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.pNext = nullptr;
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &info->commandBuffer->commandBuffer;
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	VULKAN_ERROR_CHECK(vkQueueSubmit(info->queue->queue, 1, &submitInfo, info->fence->fence));
}

void VulkanQueuePresent(SEVulkan* vulk, VulkanPresentInfo* info)
{
	VkSemaphore waitSemaphores[] = { info->waitSemaphore->semaphore };
	VkSwapchainKHR swapChains[] = { info->swapChain->swapChain };

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.pNext = nullptr;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = waitSemaphores;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &info->imageIndex;
	presentInfo.pResults = nullptr;

	vkQueuePresentKHR(info->queue->queue, &presentInfo);
}

void VulkanWaitDeviceIdle(SEVulkan* vulk)
{
	vkDeviceWaitIdle(vulk->logicalDevice);
}

void VulkanWaitQueueIdle(SEVulkanQueue* queue)
{
	vkQueueWaitIdle(queue->queue);
}
//-------------------------------------------------------------------------------------------------------------------------------------------

//RESIZE
//-------------------------------------------------------------------------------------------------------------------------------------------
void VulkanOnResize(SEVulkan* vulk, SEWindow* window, SEVulkanSwapChain* swapChain, SEVulkanPipeline* pipeline)
{
	if (vulk->instance == nullptr)
		return;

	VulkanWaitDeviceIdle(vulk);

	DestroyVulkanSwapChain(vulk, swapChain);

	CreateVulkanSwapChain(vulk, window, swapChain);

	CreateVulkanFrameBuffer(vulk, swapChain, &pipeline->renderPass);
}
//-------------------------------------------------------------------------------------------------------------------------------------------

//BUFFERS
//-------------------------------------------------------------------------------------------------------------------------------------------
uint32_t FindMemoryType(SEVulkan* vulk, uint32_t typeFilter, VkMemoryPropertyFlags requestedMemoryType)
{
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(vulk->physicalDevice, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
	{
		if (typeFilter & (1 << i) && (memProperties.memoryTypes[i].propertyFlags & requestedMemoryType) == requestedMemoryType)
		{
			return i;
		}
	}

	MessageBox(nullptr, L"Buffer does not support requested memory or the requested memory type is not supported by the GPU."
		"Exiting Prorgam!",
		L"Requested Memory Type Error.", MB_OK);
	exit(2);
}

/*void VulkanAllocateMemory(SEVulkan* vulk, SEVulkanBufferInfo* bufferInfo, SEVulkanBuffer* buffer)
{
	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(vulk->logicalDevice, buffer->buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;

	VkMemoryPropertyFlags memFlags{};
	switch (bufferInfo->access)
	{
	case SE_GPU:
		memFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		break;
	case SE_CPU_GPU:
		memFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
		break;
	}
	allocInfo.memoryTypeIndex = FindMemoryType(vulk, memRequirements.memoryTypeBits, memFlags);

	VULKAN_ERROR_CHECK(vkAllocateMemory(vulk->logicalDevice, &allocInfo, nullptr, &buffer->memory));

}*/
void VulkanCreateStagingBuffer(SEVulkan* vulk, void* srcData, VkDeviceSize size)
{
	VkBufferCreateInfo bufferCreateInfo{};
	bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferCreateInfo.pNext = nullptr;
	bufferCreateInfo.flags = 0;
	bufferCreateInfo.size = size;
	bufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	bufferCreateInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
	bufferCreateInfo.queueFamilyIndexCount = 3;
	bufferCreateInfo.pQueueFamilyIndices = vulk->familyIndices;

	/*VULKAN_ERROR_CHECK(vkCreateBuffer(vulk->logicalDevice, &bufferCreateInfo, nullptr, &gVulkanCopyEngine.stagingBuffer));

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(vulk->logicalDevice, gVulkanCopyEngine.stagingBuffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = FindMemoryType(vulk, memRequirements.memoryTypeBits,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	VULKAN_ERROR_CHECK(vkAllocateMemory(vulk->logicalDevice, &allocInfo, nullptr, &gVulkanCopyEngine.stagingBufferMemory));

	VULKAN_ERROR_CHECK(vkBindBufferMemory(vulk->logicalDevice, gVulkanCopyEngine.stagingBuffer,
		gVulkanCopyEngine.stagingBufferMemory, 0));*/

	VmaAllocationCreateInfo allocCreateInfo = {};
	allocCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
	allocCreateInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;

	vmaCreateBuffer(vulk->allocator, &bufferCreateInfo, &allocCreateInfo, &gVulkanCopyEngine.stagingBuffer,
		&gVulkanCopyEngine.stagingBufferAllocation, nullptr);

	void* data = nullptr;
	vkMapMemory(vulk->logicalDevice, gVulkanCopyEngine.stagingBufferAllocation->GetMemory(), 0, size, 0, &data);
	memcpy(data, srcData, size);
	vkUnmapMemory(vulk->logicalDevice, gVulkanCopyEngine.stagingBufferAllocation->GetMemory());
}

void VulkanDestroyStagingBuffer(SEVulkan* vulk)
{
	vmaDestroyBuffer(vulk->allocator, gVulkanCopyEngine.stagingBuffer, gVulkanCopyEngine.stagingBufferAllocation);

	//vkDestroyBuffer(vulk->logicalDevice, gVulkanCopyEngine.stagingBuffer, nullptr);
	//vkFreeMemory(vulk->logicalDevice, gVulkanCopyEngine.stagingBufferMemory, nullptr);
}

void VulkanCopyBuffer(SEVulkan* vulk, void* srcData, VkBuffer* dstBuffer, VkDeviceSize size)
{
	VulkanCreateStagingBuffer(vulk, srcData, size);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.pNext = nullptr;
	beginInfo.flags = 0;
	beginInfo.pInheritanceInfo = nullptr;

	vkBeginCommandBuffer(gVulkanCopyEngine.commandBuffer, &beginInfo);

	VkBufferCopy copyRegion{};
	copyRegion.srcOffset = 0;
	copyRegion.dstOffset = 0;
	copyRegion.size = size;
	vkCmdCopyBuffer(gVulkanCopyEngine.commandBuffer, gVulkanCopyEngine.stagingBuffer, *dstBuffer, 1, &copyRegion);

	vkEndCommandBuffer(gVulkanCopyEngine.commandBuffer);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &gVulkanCopyEngine.commandBuffer;

	vkQueueSubmit(gVulkanCopyEngine.transferQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(gVulkanCopyEngine.transferQueue);

	VulkanDestroyStagingBuffer(vulk);
}

void CreateVulkanBuffer(SEVulkan* vulk, SEVulkanBufferInfo* bufferInfo, SEVulkanBuffer* buffer)
{
	bool copyData = false;
	if (bufferInfo->data && bufferInfo->access == SE_GPU)
	{
		copyData = true;
	}

	VkBufferCreateInfo bufferCreateInfo{};
	bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferCreateInfo.pNext = nullptr;
	bufferCreateInfo.flags = 0;
	bufferCreateInfo.size = bufferInfo->size;

	switch (bufferInfo->type)
	{
	case SE_VERTEX_BUFFER:
		bufferCreateInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		break;
	case SE_INDEX_BUFFER:
		bufferCreateInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
		break;

	case SE_UNIFORM_BUFFER:
		bufferCreateInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
		break;
	}

	if (copyData)
		bufferCreateInfo.usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;

	bufferCreateInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
	bufferCreateInfo.queueFamilyIndexCount = 3;
	bufferCreateInfo.pQueueFamilyIndices = vulk->familyIndices;

	/*VULKAN_ERROR_CHECK(vkCreateBuffer(vulk->logicalDevice, &bufferCreateInfo, nullptr, &buffer->buffer));

	VulkanAllocateMemory(vulk, bufferInfo, buffer);

	VULKAN_ERROR_CHECK(vkBindBufferMemory(vulk->logicalDevice, buffer->buffer, buffer->memory, 0));*/

	VmaAllocationCreateInfo allocationInfo{};
	allocationInfo.usage = VMA_MEMORY_USAGE_AUTO;

	switch (bufferInfo->access)
	{
	case SE_GPU:
		allocationInfo.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
		break;
	case SE_CPU_GPU:
		allocationInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
		break;
	}

	vmaCreateBuffer(vulk->allocator, &bufferCreateInfo, &allocationInfo, &buffer->buffer, 
		&buffer->allocation, nullptr);

	if (copyData)
		VulkanCopyBuffer(vulk, bufferInfo->data, &buffer->buffer, bufferInfo->size);
}

void DestroyVulkanBuffer(SEVulkan* vulk, SEVulkanBuffer* buffer)
{
	vmaDestroyBuffer(vulk->allocator, buffer->buffer, buffer->allocation);

	//vkDestroyBuffer(vulk->logicalDevice, buffer->buffer, nullptr);
	//vkFreeMemory(vulk->logicalDevice, buffer->memory, nullptr);
}

void VulkanMapMemory(SEVulkan* vulk, SEVulkanBuffer* buffer, uint32_t offset, uint32_t size, void** data)
{
	vmaMapMemory(vulk->allocator, buffer->allocation, data);
	//vkMapMemory(vulk->logicalDevice, buffer->allocation->GetMemory(), offset, size, 0, data);
}

void VulkanUnmapMemory(SEVulkan* vulk, SEVulkanBuffer* buffer)
{
	vmaUnmapMemory(vulk->allocator, buffer->allocation);
	//vkUnmapMemory(vulk->logicalDevice, buffer->allocation->GetMemory());
}

void VulkanBindBuffer(SEVulkanCommandBuffer* commandBuffer, uint32_t bindingLocation,
	SEVulkanBuffer* buffer, uint32_t offset, SEVulkanBufferType bufferType)
{
	VkDeviceSize offs = offset;
	switch (bufferType)
	{
	case SE_VERTEX_BUFFER:
		vkCmdBindVertexBuffers(commandBuffer->commandBuffer, bindingLocation, 1, &buffer->buffer, &offs);
		break;

	case SE_INDEX_BUFFER:
		vkCmdBindIndexBuffer(commandBuffer->commandBuffer, buffer->buffer, offs, VK_INDEX_TYPE_UINT32);
		break;
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------------

//DESCRIPTORS
//-------------------------------------------------------------------------------------------------------------------------------------------
void CreateVulkanDescriptorSets(SEVulkan* vulk, SEVulkanDescriptorSetInfo* info, SEVulkanDescriptorSet* ds)
{
	for (uint32_t i = 0; i < info->numDescriptors; ++i)
	{
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.pNext = nullptr;
		allocInfo.descriptorPool = gDescriptorPool;
		allocInfo.descriptorSetCount = 1;
		allocInfo.pSetLayouts = &info->layout->descriptorSetLayout;

		VULKAN_ERROR_CHECK(vkAllocateDescriptorSets(vulk->logicalDevice, &allocInfo, &ds[i].descriptorSet));
	} 
}

void UpdateVulkanDescriptorSet(SEVulkan* vulk, SEVulkanUpdateDescriptorSetInfo* info, SEVulkanDescriptorSet* ds)
{
	if (info->bufferInfo.buffer)
	{
		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = info->bufferInfo.buffer->buffer;
		bufferInfo.offset = info->bufferInfo.offset;
		bufferInfo.range = info->bufferInfo.range;

		VkWriteDescriptorSet descriptorWrite{};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.pNext = nullptr;
		descriptorWrite.dstSet = ds->descriptorSet;
		descriptorWrite.dstBinding = info->binding;
		descriptorWrite.dstArrayElement = info->firstArrayElement;
		descriptorWrite.descriptorCount = info->numArrayElements;
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrite.pBufferInfo = &bufferInfo;

		vkUpdateDescriptorSets(vulk->logicalDevice, 1, &descriptorWrite, 0, nullptr);
	}
}

void VulkanBindDescriptorSet(SEVulkanCommandBuffer* commandBuffer, SEPipelineType type, SEVulkanPipeline* pipeline, SEVulkanDescriptorSet* ds)
{
	VkPipelineBindPoint bindPoint{};
	switch (type)
	{
	case SE_GRAPHICS_PIPELINE:
		bindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		break;

	case SE_COMPUTE_PIPELINE:
		bindPoint = VK_PIPELINE_BIND_POINT_COMPUTE;
		break;
	}

	vkCmdBindDescriptorSets(commandBuffer->commandBuffer, bindPoint, pipeline->pipelineLayout, 0, 1, &ds->descriptorSet, 0, nullptr);
}
//-------------------------------------------------------------------------------------------------------------------------------------------