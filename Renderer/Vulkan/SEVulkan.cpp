#define VMA_IMPLEMENTATION
#include "SEVulkan.h"
#include "..\SEDDSLoader.h"

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

void SetupDebugMessenger(Vulkan* vulk)
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

bool CheckDeviceExtensionSupport(const Vulkan* vulk, const char** extensionNames, uint32_t numExtensions)
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
void CreateInstance(Vulkan* vulk, const char* appName)
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

float gMaxSamplerAnisotropy = 0.0f;
void CreatePhysicalDevice(Vulkan* vulk)
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
			gMaxSamplerAnisotropy = deviceProperties.limits.maxSamplerAnisotropy;
			return;
		}
	}

	MessageBox(nullptr, L"Couldn't find a discrete gpu. Exiting Program.", L"No discrete GPU found.", MB_OK);
	exit(2);
}

#define GRAPHICS_FAM_INDEX 0
#define COMPUTE_FAM_INDEX 1
#define TRANSFER_FAM_INDEX 2

void FindQueueFamilies(Vulkan* vulk)
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

void CreateLogicalDevice(Vulkan* vulk)
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

	VkPhysicalDeviceFeatures deviceFeatures{};
	deviceFeatures.samplerAnisotropy = VK_TRUE;

	const char* extensions[] = { "VK_KHR_swapchain", "VK_KHR_dynamic_rendering"};
	if (!CheckDeviceExtensionSupport(vulk, extensions, 2))
	{
		MessageBox(nullptr, L"One or more device extensions are not avaiable. Exiting Program.", L"Device extensions error.", MB_OK);
		free(queueCreateInfos);
		exit(2);
	}

	VkPhysicalDeviceDynamicRenderingFeaturesKHR dynamicRenderingFeatures{};
	dynamicRenderingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR;
	dynamicRenderingFeatures.pNext = nullptr;
	dynamicRenderingFeatures.dynamicRendering = VK_TRUE;

	VkDeviceCreateInfo deviceCreateInfo{};
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.pNext = &dynamicRenderingFeatures;
	deviceCreateInfo.queueCreateInfoCount = 3;
	deviceCreateInfo.pQueueCreateInfos = queueCreateInfos;
	deviceCreateInfo.enabledExtensionCount = 2;
	deviceCreateInfo.ppEnabledExtensionNames = extensions;
	deviceCreateInfo.pEnabledFeatures = &deviceFeatures;

	VULKAN_ERROR_CHECK(vkCreateDevice(vulk->physicalDevice, &deviceCreateInfo, nullptr, &vulk->logicalDevice));

	free(queueCreateInfos);
}

void CreateSurface(Vulkan* vulk, const Window* const window)
{
	VkWin32SurfaceCreateInfoKHR surfaceInfo{};
	surfaceInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	surfaceInfo.pNext = nullptr;
	surfaceInfo.flags = 0;
	surfaceInfo.hinstance = GetModuleHandle(nullptr);
	surfaceInfo.hwnd = window->wndHandle;

	VULKAN_ERROR_CHECK(vkCreateWin32SurfaceKHR(vulk->instance, &surfaceInfo, nullptr, &vulk->surface));
}

void VmaInit(Vulkan* vulk)
{
	VmaAllocatorCreateInfo allocatorCreateInfo = {};
	allocatorCreateInfo.flags = 0;
	allocatorCreateInfo.vulkanApiVersion = VK_API_VERSION_1_3;
	allocatorCreateInfo.physicalDevice = vulk->physicalDevice;
	allocatorCreateInfo.device = vulk->logicalDevice;
	allocatorCreateInfo.instance = vulk->instance;

	vmaCreateAllocator(&allocatorCreateInfo, &vulk->allocator);
}

void VmaDestroy(Vulkan* vulk)
{
	vmaDestroyAllocator(vulk->allocator);
}

struct VulkanCopyEngine
{
	VkQueue transferQueue;
	VkCommandPool commandPool;
	VkCommandBuffer commandBuffer;
	VkBuffer stagingBuffer;
	VmaAllocation stagingBufferAllocation;
}gVulkanCopyEngine;

void VulkanInitCopyEngine(const Vulkan* const vulk)
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

void VulkanBeginCopyEngineCommandBuffer()
{
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.pNext = nullptr;
	beginInfo.flags = 0;
	beginInfo.pInheritanceInfo = nullptr;

	vkBeginCommandBuffer(gVulkanCopyEngine.commandBuffer, &beginInfo);
}

void VulkanEndCopyEngineCommandBuffer()
{
	VULKAN_ERROR_CHECK(vkEndCommandBuffer(gVulkanCopyEngine.commandBuffer));

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &gVulkanCopyEngine.commandBuffer;

	vkQueueSubmit(gVulkanCopyEngine.transferQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(gVulkanCopyEngine.transferQueue);
}

void VulkanDestroyCopyEngine(const Vulkan* const vulk)
{
	vkDestroyCommandPool(vulk->logicalDevice, gVulkanCopyEngine.commandPool, nullptr);
}

#define MAX_NUM_DESCRIPTORS 16
VkDescriptorPool gDescriptorPool;
void VulkanCreateDescriptorPool(const Vulkan* const vulk)
{
	VkDescriptorPoolSize poolSize[3]{};

	poolSize[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSize[0].descriptorCount = MAX_NUM_DESCRIPTORS;

	poolSize[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSize[1].descriptorCount = MAX_NUM_DESCRIPTORS;

	poolSize[2].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	poolSize[2].descriptorCount = MAX_NUM_DESCRIPTORS;

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.pNext = nullptr;
	poolInfo.flags = 0;
	poolInfo.maxSets = MAX_NUM_DESCRIPTORS;
	poolInfo.poolSizeCount = 3;
	poolInfo.pPoolSizes = poolSize;

	VULKAN_ERROR_CHECK(vkCreateDescriptorPool(vulk->logicalDevice, &poolInfo, nullptr, &gDescriptorPool));
}

void VulkanDestroyDescriptorPool(const Vulkan* const vulk)
{
	vkDestroyDescriptorPool(vulk->logicalDevice, gDescriptorPool, nullptr);
}

void VulkanInit(Vulkan* vulk, const Window* const window, const char* appName)
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

	VmaInit(vulk);

	VulkanInitCopyEngine(vulk);

	VulkanCreateDescriptorPool(vulk);
}

void VulkanDestroy(Vulkan* vulk)
{
	VulkanDestroyDescriptorPool(vulk);

	VulkanDestroyCopyEngine(vulk);

	VmaDestroy(vulk);

	vkDestroyDevice(vulk->logicalDevice, nullptr);

	if (vulk->enableValidationLayers)
	{
		DestroyDebugUtilsMessengerEXT(vulk->instance, vulk->debugMessenger, nullptr);
	}

	vkDestroySurfaceKHR(vulk->instance, vulk->surface, nullptr);
	vkDestroyInstance(vulk->instance, nullptr);
}
//-------------------------------------------------------------------------------------------------------------------------------------------

void VulkanGetQueueHandle(const Vulkan* const vulk, const VulkanQueueInfo* const queueInfo, VulkanQueue* queue)
{
	switch (queueInfo->queueType)
	{
	case QUEUE_TYPE_GRAPHICS:
		vkGetDeviceQueue(vulk->logicalDevice, vulk->familyIndices[GRAPHICS_FAM_INDEX], queueInfo->index, &queue->queue);
		return;

	case QUEUE_TYPE_COMPUTE:
		vkGetDeviceQueue(vulk->logicalDevice, vulk->familyIndices[COMPUTE_FAM_INDEX], queueInfo->index, &queue->queue);
		return;
	}
}

void VulkanCreateRenderingAttachment(const Vulkan* const vulk,
	const VulkanRenderAttachmentInfo* const info, VulkanRenderAttachment* attachment)
{
	attachment->clearValue = info->clearValue;
	attachment->format = info->format;
	attachment->width = info->width;
	attachment->height = info->height;

	bool const isDepth = TinyImageFormat_IsDepthOnly(info->format) || TinyImageFormat_IsDepthAndStencil(info->format);

	VkImageCreateInfo createImageInfo{};
	createImageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	createImageInfo.pNext = nullptr;
	createImageInfo.flags = 0;
	createImageInfo.imageType = VK_IMAGE_TYPE_2D;
	createImageInfo.format = (VkFormat)TinyImageFormat_ToVkFormat(info->format);
	createImageInfo.extent.width = info->width;
	createImageInfo.extent.height = info->height;
	createImageInfo.extent.depth = 1;
	createImageInfo.mipLevels = 1;
	createImageInfo.arrayLayers = 1;
	createImageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	createImageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;

	if (isDepth)
		createImageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	else
		createImageInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	createImageInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
	createImageInfo.queueFamilyIndexCount = 3;
	createImageInfo.pQueueFamilyIndices = vulk->familyIndices;
	createImageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

	VmaAllocationCreateInfo allocCreateInfo = {};
	allocCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
	allocCreateInfo.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
	allocCreateInfo.priority = 1.0f;

	VULKAN_ERROR_CHECK(vmaCreateImage(vulk->allocator, &createImageInfo,
		&allocCreateInfo, &attachment->image, &attachment->allocation, nullptr));

	VkImageViewCreateInfo createImageViewInfo{};
	createImageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	createImageViewInfo.pNext = nullptr;
	createImageViewInfo.flags = 0;
	createImageViewInfo.image = attachment->image;
	createImageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	createImageViewInfo.format = (VkFormat)TinyImageFormat_ToVkFormat(info->format);
	createImageViewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	createImageViewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	createImageViewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	createImageViewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

	if (isDepth)
		createImageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
	else
		createImageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

	createImageViewInfo.subresourceRange.baseMipLevel = 0;
	createImageViewInfo.subresourceRange.levelCount = 1;
	createImageViewInfo.subresourceRange.baseArrayLayer = 0;
	createImageViewInfo.subresourceRange.layerCount = 1;

	VULKAN_ERROR_CHECK(vkCreateImageView(vulk->logicalDevice, &createImageViewInfo, nullptr, &attachment->imageView));
}

void VulkanDestroyRenderingAttachment(const Vulkan* const vulk, VulkanRenderAttachment* attachment)
{
	vkDestroyImageView(vulk->logicalDevice, attachment->imageView, nullptr);
	vmaDestroyImage(vulk->allocator, attachment->image, attachment->allocation);
}

//SWAPCHAIN
//-------------------------------------------------------------------------------------------------------------------------------------------
struct VulkanInternalSwapChainQueryInfo
{
	VkSurfaceCapabilitiesKHR capabilities;

	uint32_t formatCount;
	VkSurfaceFormatKHR* formats;

	uint32_t presentModeCount;
	VkPresentModeKHR* presentModes;
};

void VulkanFreeSwapChainInfo(VulkanInternalSwapChainQueryInfo* queryInfo)
{
	free(queryInfo->formats);
	free(queryInfo->presentModes);
}

void QuerySwapChainSupport(const Vulkan* const vulk, VulkanInternalSwapChainQueryInfo* queryInfo)
{
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vulk->physicalDevice, vulk->surface, &queryInfo->capabilities);

	vkGetPhysicalDeviceSurfaceFormatsKHR(vulk->physicalDevice, vulk->surface, &queryInfo->formatCount, nullptr);

	if (queryInfo->formatCount == 0)
	{
		MessageBox(nullptr, L"No surface formats are supported by the GPU. Exiting Program.", L"Surface formats error.", MB_OK);
		exit(2);
	}
	queryInfo->formats = (VkSurfaceFormatKHR*)calloc(queryInfo->formatCount, sizeof(VkSurfaceFormatKHR));
	vkGetPhysicalDeviceSurfaceFormatsKHR(vulk->physicalDevice, vulk->surface, &queryInfo->formatCount, queryInfo->formats);

	vkGetPhysicalDeviceSurfacePresentModesKHR(vulk->physicalDevice, vulk->surface, &queryInfo->presentModeCount, nullptr);
	if (queryInfo->presentModeCount == 0)
	{
		MessageBox(nullptr, L"No present modes are supported by the GPU. Exiting Program.", L"Present modes error.", MB_OK);
		free(queryInfo->formats);
		exit(2);
	}
	queryInfo->presentModes = (VkPresentModeKHR*)calloc(queryInfo->presentModeCount, sizeof(VkPresentModeKHR));
	vkGetPhysicalDeviceSurfacePresentModesKHR(vulk->physicalDevice, vulk->surface, 
		&queryInfo->presentModeCount, queryInfo->presentModes);
}

VkSurfaceFormatKHR CheckSwapSurfaceFormat(const VulkanInternalSwapChainQueryInfo* const queryInfo,
	const VulkanSwapChainInfo* const info)
{
	VkFormat format = (VkFormat)TinyImageFormat_ToVkFormat(info->format);
	for (uint32_t i = 0; i < queryInfo->formatCount; ++i)
	{
		if (queryInfo->formats[i].format == format &&
			queryInfo->formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			return queryInfo->formats[i];
	}

	MessageBox(nullptr, L"Specified format is not supported by your GPU. Exiting Program.", L"Invalid Format.", MB_OK);
	exit(2);
}

VkPresentModeKHR ChooseSwapPresentMode(const VulkanInternalSwapChainQueryInfo* const queryInfo)
{
	for (uint32_t i = 0; i < queryInfo->presentModeCount; ++i)
	{
		if (queryInfo->presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
			return queryInfo->presentModes[i];
	}

	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D CheckSwapExtent(const VulkanInternalSwapChainQueryInfo* queryInfo, const VulkanSwapChainInfo* const info)
{
	VkExtent2D extent{};

	extent.width = info->width;
	extent.height = info->height;

	if (extent.width < queryInfo->capabilities.minImageExtent.width)
	{
		extent.width = queryInfo->capabilities.minImageExtent.width;
	}
	else if (extent.width > queryInfo->capabilities.maxImageExtent.width)
	{
		extent.width = queryInfo->capabilities.maxImageExtent.width;
	}

	if (extent.height < queryInfo->capabilities.minImageExtent.height)
	{
		extent.height = queryInfo->capabilities.minImageExtent.height;
	}
	else if (extent.height > queryInfo->capabilities.maxImageExtent.height)
	{
		extent.height = queryInfo->capabilities.maxImageExtent.height;
	}

	return extent;
}

void VulkanCreateSwapChain(const Vulkan* const vulk, const VulkanSwapChainInfo* const info, VulkanSwapChain* swapChain)
{
	swapChain->window = info->window;
	swapChain->clearValue = info->clearValue;

	VulkanInternalSwapChainQueryInfo queryInfo{};
	QuerySwapChainSupport(vulk, &queryInfo);

	VkSurfaceFormatKHR surfaceFormat = CheckSwapSurfaceFormat(&queryInfo, info);
	VkPresentModeKHR presentMode = ChooseSwapPresentMode(&queryInfo);
	VkExtent2D extent = CheckSwapExtent(&queryInfo, info);

	uint32_t imageCount = queryInfo.capabilities.minImageCount + 1;
	if (queryInfo.capabilities.maxImageCount > 0 && imageCount > queryInfo.capabilities.maxImageCount)
	{
		imageCount = queryInfo.capabilities.maxImageCount;
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
	swapCreateInfo.preTransform = queryInfo.capabilities.currentTransform;
	swapCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	swapCreateInfo.presentMode = presentMode;
	swapCreateInfo.clipped = VK_TRUE;
	swapCreateInfo.oldSwapchain = VK_NULL_HANDLE;

	VULKAN_ERROR_CHECK(vkCreateSwapchainKHR(vulk->logicalDevice, &swapCreateInfo, nullptr, &swapChain->swapChain));

	vkGetSwapchainImagesKHR(vulk->logicalDevice, swapChain->swapChain, &swapChain->imageCount, nullptr);
	swapChain->colorAttachments = (VulkanRenderAttachment*)calloc(swapChain->imageCount, sizeof(VulkanRenderAttachment));
	VkImage* images = (VkImage*)calloc(swapChain->imageCount, sizeof(VkImage));
	vkGetSwapchainImagesKHR(vulk->logicalDevice, swapChain->swapChain, &swapChain->imageCount, images);

	for (uint32_t i = 0; i < swapChain->imageCount; ++i)
	{
		swapChain->colorAttachments[i].image = images[i];
		swapChain->colorAttachments[i].width = extent.width;
		swapChain->colorAttachments[i].height = extent.height;
	}
	free(images);

	swapChain->format = TinyImageFormat_FromVkFormat((TinyImageFormat_VkFormat)surfaceFormat.format);

	swapChain->width = extent.width;
	swapChain->height = extent.height;

	VulkanFreeSwapChainInfo(&queryInfo);

	for (uint32_t i = 0; i < swapChain->imageCount; ++i)
	{
		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.pNext = nullptr;
		createInfo.flags = 0;
		createInfo.image = swapChain->colorAttachments[i].image;
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = (VkFormat)TinyImageFormat_ToVkFormat(swapChain->format);
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		VULKAN_ERROR_CHECK(vkCreateImageView(vulk->logicalDevice, &createInfo, nullptr, &swapChain->colorAttachments[i].imageView));
	}
}

void VulkanDestroySwapChain(const Vulkan* const vulk, VulkanSwapChain* swapChain)
{
	for (uint32_t i = 0; i < swapChain->imageCount; ++i)
	{
		vkDestroyImageView(vulk->logicalDevice, swapChain->colorAttachments[i].imageView, nullptr);
	}
	free(swapChain->colorAttachments);
	vkDestroySwapchainKHR(vulk->logicalDevice, swapChain->swapChain, nullptr);
}
//-------------------------------------------------------------------------------------------------------------------------------------------

//SHADERS
//-------------------------------------------------------------------------------------------------------------------------------------------
void VulkanCreateShader(const Vulkan* const vulk, const VulkanShaderInfo* const info, VulkanShader* shader)
{
	char* buffer = nullptr;
	uint32_t fileSize = 0;
	ReadFile(info->filename, &buffer, &fileSize, BINARY);
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
	switch (info->type)
	{
	case SHADER_TYPE_VERTEX:
		stageCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		break;

	case SHADER_TYPE_PIXEL:
		stageCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		break;

	case SHADER_TYPE_COMPUTE:
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

void VulkanDestroyShader(const Vulkan* const vulk, VulkanShader* shader)
{
	vkDestroyShaderModule(vulk->logicalDevice, shader->shaderModule, nullptr);
}
//-------------------------------------------------------------------------------------------------------------------------------------------

//PIPELINE
//-------------------------------------------------------------------------------------------------------------------------------------------
void VulkanCreateInputAssemblyInfo(const VulkanPipleineInfo* const info, VkPipelineInputAssemblyStateCreateInfo* inputAssemblyInfo)
{
	inputAssemblyInfo->sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssemblyInfo->pNext = nullptr;
	inputAssemblyInfo->flags = 0;

	switch (info->topology)
	{
	case TOPOLOGY_POINT_LIST:
		inputAssemblyInfo->topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
		break;

	case TOPOLOGY_LINE_LIST:
		inputAssemblyInfo->topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
		break;

	case TOPOLOGY_LINE_STRIP:
		inputAssemblyInfo->topology = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
		break;

	case TOPOLOGY_TRIANGLE_LIST:
		inputAssemblyInfo->topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		break;

	case TOPOLOGY_TRIANGLE_STRIP:
		inputAssemblyInfo->topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
		break;
	}

	inputAssemblyInfo->primitiveRestartEnable = VK_FALSE;
}

void VulkanCreateRasterizerInfo(const VulkanPipleineInfo* const info, VkPipelineRasterizationStateCreateInfo* rasInfo)
{
	rasInfo->sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasInfo->pNext = nullptr;
	rasInfo->flags = 0;
	rasInfo->depthClampEnable = VK_FALSE;
	rasInfo->rasterizerDiscardEnable = VK_FALSE;

	switch (info->rasInfo.cullMode)
	{
	case CULL_MODE_NONE:
		rasInfo->cullMode = VK_CULL_MODE_NONE;
		break;

	case CULL_MODE_FRONT:
		rasInfo->cullMode = VK_CULL_MODE_FRONT_BIT;
		break;

	case CULL_MODE_BACK:
		rasInfo->cullMode = VK_CULL_MODE_BACK_BIT;
		break;
	}

	switch (info->rasInfo.faceMode)
	{
	case FACE_CLOCKWISE:
		rasInfo->frontFace = VK_FRONT_FACE_CLOCKWISE;
		break;

	case FACE_COUNTER_CLOCKWISE:
		rasInfo->frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		break;
	}

	switch (info->rasInfo.polygonMode)
	{
	case POLYGON_MODE_FILL:
		rasInfo->polygonMode = VK_POLYGON_MODE_FILL;
		break;
	case POLYGON_MODE_WIREFRAME:
		rasInfo->polygonMode = VK_POLYGON_MODE_LINE;
		break;
	}

	rasInfo->depthBiasEnable = VK_FALSE;
	rasInfo->depthBiasConstantFactor = 0.0f;
	rasInfo->depthBiasClamp = 0.0f;
	rasInfo->depthBiasSlopeFactor = 0.0f;
	rasInfo->lineWidth = info->rasInfo.lineWidth;
}

void VulkanCreateBindingDescription(const VulkanVertexBindingInfo* const bindingInfo, VkVertexInputBindingDescription* desc)
{
	desc->binding = bindingInfo->binding;
	desc->stride = bindingInfo->stride;

	switch (bindingInfo->inputRate)
	{
	case INPUT_RATE_PER_VERTEX:
		desc->inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		break;

	case INPUT_RATE_PER_INSTANCE:
		desc->inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;
		break;
	}
}

void VulkanCreateAttributeDescription(const VulkanAttributeInfo* const attribInfo, VkVertexInputAttributeDescription* desc, uint32_t numInfos)
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

void VulkanCreateVertexDescriptions(const VulkanPipleineInfo* const pipelineInfo, 
	VkVertexInputBindingDescription* bindingDesc, VkVertexInputAttributeDescription* attribDesc,
	VkPipelineVertexInputStateCreateInfo* createInfo)
{
	VulkanCreateBindingDescription(&pipelineInfo->bindingInfo, bindingDesc);

	VulkanCreateAttributeDescription(pipelineInfo->attributeInfo, attribDesc, pipelineInfo->numAttributes);

	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	createInfo->sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	createInfo->vertexBindingDescriptionCount = 1;
	createInfo->pVertexBindingDescriptions = bindingDesc;
	createInfo->vertexAttributeDescriptionCount = pipelineInfo->numAttributes;
	createInfo->pVertexAttributeDescriptions = attribDesc;
}

void VulkanCreateDescriptorSetLayout(const Vulkan* const vulk, const VulkanDescritporSetLayoutInfo* const info,
	VulkanDescriptorSetLayout* setLayout)
{
	VkDescriptorSetLayoutBinding layoutBinding[MAX_NUM_DESCRIPTOR_SET_LAYOUT_BINDINGS]{};
	for (uint32_t i = 0; i < info->numBindings; ++i)
	{
		layoutBinding[i].binding = info->bindingInfo[i].binding;

		switch (info->bindingInfo[i].type)
		{
		case DESCRITPTOR_TYPE_UNIFORM_BUFFER:
			layoutBinding[i].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			break;

		case DESCRITPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
			layoutBinding[i].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			break;

		case DESCRITPTOR_TYPE_STORAGE_BUFFER:
			layoutBinding[i].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			break;
		}

		layoutBinding[i].descriptorCount = info->bindingInfo[i].numDescriptors;

		if (info->bindingInfo[i].stages & STAGE_VERTEX)
		{
			layoutBinding[i].stageFlags |= VK_SHADER_STAGE_VERTEX_BIT;
		}

		if (info->bindingInfo[i].stages & STAGE_PIXEL)
		{
			layoutBinding[i].stageFlags |= VK_SHADER_STAGE_FRAGMENT_BIT;
		}

		if (info->bindingInfo[i].stages & STAGE_COMPUTE)
		{
			layoutBinding[i].stageFlags |= VK_SHADER_STAGE_COMPUTE_BIT;
		}

		layoutBinding[i].pImmutableSamplers = nullptr;
	}

	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.pNext = nullptr;
	layoutInfo.flags = 0;
	layoutInfo.bindingCount = info->numBindings;
	layoutInfo.pBindings = layoutBinding;

	VULKAN_ERROR_CHECK(vkCreateDescriptorSetLayout(vulk->logicalDevice, &layoutInfo, nullptr, &setLayout->descriptorSetLayout))
}

void VulkanCreatePipelineLayout(const Vulkan* const vulk, const VulkanPipleineInfo* const info, VulkanPipeline* pipeline)
{
	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = &info->descriptorSetLayout->descriptorSetLayout;
	pipelineLayoutInfo.pushConstantRangeCount = 0;
	pipelineLayoutInfo.pPushConstantRanges = nullptr;

	VULKAN_ERROR_CHECK(vkCreatePipelineLayout(vulk->logicalDevice, &pipelineLayoutInfo, nullptr, &pipeline->pipelineLayout));
}

void VulkanDestroyDescriptorSetLayout(const Vulkan* const vulk, VulkanDescriptorSetLayout* setLayout)
{
	vkDestroyDescriptorSetLayout(vulk->logicalDevice, setLayout->descriptorSetLayout, nullptr);
}

void VulkanCreateBlendInfo(const VulkanPipleineInfo* const info, VkPipelineColorBlendAttachmentState* blendState,
	VkPipelineColorBlendStateCreateInfo* blendStateCreateInfo)
{
	blendState->colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	blendState->blendEnable = info->blendInfo.enableBlend;
	blendState->srcColorBlendFactor = (VkBlendFactor)info->blendInfo.srcColorBlendFactor;
	blendState->dstColorBlendFactor = (VkBlendFactor)info->blendInfo.dstColorBlendFactor;
	blendState->colorBlendOp = (VkBlendOp)info->blendInfo.colorBlendOp;
	blendState->srcAlphaBlendFactor = (VkBlendFactor)info->blendInfo.srcAlphaBlendFactor;
	blendState->dstAlphaBlendFactor = (VkBlendFactor)info->blendInfo.dstAlphaBlendFactor;
	blendState->alphaBlendOp = (VkBlendOp)info->blendInfo.alphaBlendOp;
	blendState->colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT
		| VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

	VkPipelineColorBlendStateCreateInfo colorBlending{};
	blendStateCreateInfo->sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	blendStateCreateInfo->logicOpEnable = VK_FALSE;
	blendStateCreateInfo->logicOp = VK_LOGIC_OP_COPY;
	blendStateCreateInfo->attachmentCount = 1;
	blendStateCreateInfo->pAttachments = blendState;
	blendStateCreateInfo->blendConstants[0] = info->blendInfo.blendConstants[0];
	blendStateCreateInfo->blendConstants[1] = info->blendInfo.blendConstants[1];
	blendStateCreateInfo->blendConstants[2] = info->blendInfo.blendConstants[2];
	blendStateCreateInfo->blendConstants[3] = info->blendInfo.blendConstants[3];
}

void VulkanCreateGraphicsPipeline(const Vulkan* const vulk, const VulkanPipleineInfo* const info, VulkanPipeline* pipeline)
{
	VkVertexInputBindingDescription bindingDesc{};
	VkVertexInputAttributeDescription attribDesc[NUM_ATTRIBUTES]{};
	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	VulkanCreateVertexDescriptions(info, &bindingDesc, attribDesc, &vertexInputInfo);

	VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};
	VulkanCreateInputAssemblyInfo(info, &inputAssemblyInfo);

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
	VulkanCreateRasterizerInfo(info, &rasInfo);

	VkPipelineMultisampleStateCreateInfo multisampling{};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampling.minSampleShading = 1.0f;
	multisampling.pSampleMask = nullptr;
	multisampling.alphaToCoverageEnable = VK_FALSE;
	multisampling.alphaToOneEnable = VK_FALSE;

	VkPipelineColorBlendAttachmentState blendState{};
	VkPipelineColorBlendStateCreateInfo blendStateCreateInfo{};
	VulkanCreateBlendInfo(info, &blendState, &blendStateCreateInfo);

	VulkanCreatePipelineLayout(vulk, info, pipeline);

	VkPipelineDepthStencilStateCreateInfo depthStencil{};
	depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencil.pNext = nullptr;
	depthStencil.flags = 0;
	depthStencil.depthTestEnable = info->depthInfo.depthTest;
	depthStencil.depthWriteEnable = info->depthInfo.depthWrite;
	depthStencil.depthCompareOp = (VkCompareOp)info->depthInfo.depthFunction;
	depthStencil.depthBoundsTestEnable = VK_FALSE;
	depthStencil.stencilTestEnable = VK_FALSE;
	depthStencil.front = {};
	depthStencil.back = {};
	depthStencil.minDepthBounds = 0.0f;
	depthStencil.maxDepthBounds = 1.0f;

	VkPipelineShaderStageCreateInfo shaders[] = { info->shaders[0].shaderCreateInfo, info->shaders[1].shaderCreateInfo };

	VkPipelineRenderingCreateInfo pipelineRendingerCreateInfo{};
	pipelineRendingerCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR;
	pipelineRendingerCreateInfo.pNext = nullptr;
	pipelineRendingerCreateInfo.viewMask = 0;
	pipelineRendingerCreateInfo.colorAttachmentCount = 1;
	VkFormat colorFormat = (VkFormat)TinyImageFormat_ToVkFormat(info->colorFormat);
	pipelineRendingerCreateInfo.pColorAttachmentFormats = &colorFormat;

	if (info->depthInfo.depthTest)
	{
		pipelineRendingerCreateInfo.depthAttachmentFormat = (VkFormat)TinyImageFormat_ToVkFormat(info->depthFormat);
	}

	VkGraphicsPipelineCreateInfo pipelineInfo{};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.pNext = &pipelineRendingerCreateInfo;
	pipelineInfo.flags = 0;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaders;
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssemblyInfo;
	pipelineInfo.pTessellationState = nullptr;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasInfo;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pDepthStencilState = &depthStencil;
	pipelineInfo.pColorBlendState = &blendStateCreateInfo;
	pipelineInfo.pDynamicState = &dynamicStateInfo;
	pipelineInfo.layout = pipeline->pipelineLayout;
	pipelineInfo.renderPass = nullptr;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; 
	pipelineInfo.basePipelineIndex = -1;

	VULKAN_ERROR_CHECK(vkCreateGraphicsPipelines(vulk->logicalDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline->pipeline));
}

void VulkanCreateComputePipeline(const Vulkan* const vulk, const VulkanPipleineInfo* const info, VulkanPipeline* pipeline)
{
	VulkanCreatePipelineLayout(vulk, info, pipeline);

	VkComputePipelineCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.flags = 0;
	createInfo.stage = info->shaders[0].shaderCreateInfo;
	createInfo.layout = pipeline->pipelineLayout;
	createInfo.basePipelineHandle = nullptr;
	createInfo.basePipelineIndex = 0;

	VULKAN_ERROR_CHECK(vkCreateComputePipelines(vulk->logicalDevice, VK_NULL_HANDLE, 1, &createInfo, nullptr, &pipeline->pipeline));
}

void VulkanCreatePipeline(const Vulkan* const vulk, const VulkanPipleineInfo* const info, VulkanPipeline* pipeline)
{
	pipeline->type = info->type;

	if (info->type == PIPELINE_TYPE_GRAPHICS)
	{
		VulkanCreateGraphicsPipeline(vulk, info, pipeline);
	}
	else if (info->type == PIPELINE_TYPE_COMPUTE)
	{
		VulkanCreateComputePipeline(vulk, info, pipeline);
	}
}

void VulkanDestroyPipeline(const Vulkan* const vulk, VulkanPipeline* pipeline)
{
	vkDestroyPipeline(vulk->logicalDevice, pipeline->pipeline, nullptr);
	vkDestroyPipelineLayout(vulk->logicalDevice, pipeline->pipelineLayout, nullptr);
	vkDestroyRenderPass(vulk->logicalDevice, pipeline->renderPass, nullptr);
}
//-------------------------------------------------------------------------------------------------------------------------------------------

//COMMAND BUFFERS
//-------------------------------------------------------------------------------------------------------------------------------------------
void VulkanCreateCommandPool(const Vulkan* const vulk, VulkanCommandPool* commandPool, const VulkanQueueType queueType)
{
	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.pNext = nullptr;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

	switch (queueType)
	{
	case QUEUE_TYPE_GRAPHICS:
		poolInfo.queueFamilyIndex = vulk->familyIndices[GRAPHICS_FAM_INDEX];
		break;

	case QUEUE_TYPE_COMPUTE:
		poolInfo.queueFamilyIndex = vulk->familyIndices[COMPUTE_FAM_INDEX];
		break;
	}

	VULKAN_ERROR_CHECK(vkCreateCommandPool(vulk->logicalDevice, &poolInfo, nullptr, &commandPool->commandPool));
}

void VulkanDestroyCommandPool(const Vulkan* const vulk, VulkanCommandPool* commandPool)
{
	vkDestroyCommandPool(vulk->logicalDevice, commandPool->commandPool, nullptr);
}

void VulkanCreateCommandBuffer(const Vulkan* const vulk, const VulkanCommandPool* const commandPool,
	VulkanCommandBuffer* commandBuffer, const uint32_t numCommandBuffers)
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

void VulkanResetCommandBuffer(const VulkanCommandBuffer* const commandBuffer)
{
	VULKAN_ERROR_CHECK(vkResetCommandBuffer(commandBuffer->commandBuffer, 0));
}

void VulkanBeginCommandBuffer(const VulkanCommandBuffer* const commandBuffer)
{
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.pNext = nullptr;
	beginInfo.flags = 0;
	beginInfo.pInheritanceInfo = nullptr;

	VULKAN_ERROR_CHECK(vkBeginCommandBuffer(commandBuffer->commandBuffer, &beginInfo));
}

void VulkanBindRenderAttachment(VulkanCommandBuffer* commandBuffer, const VulkanBindRenderingAttachmentInfo* const info)
{
	if (!info)
	{
		if (commandBuffer->isRendering)
		{
			vkCmdEndRendering(commandBuffer->commandBuffer);
			commandBuffer->isRendering = false;
		}

		return;
	}

	VkRenderingAttachmentInfoKHR colorAttachment{};
	colorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
	colorAttachment.pNext = nullptr;
	colorAttachment.imageView = info->colorAttachment->imageView;
	colorAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	colorAttachment.resolveMode = VK_RESOLVE_MODE_NONE;
	colorAttachment.loadOp = (VkAttachmentLoadOp)info->colorAttachmentLoadOp;
	colorAttachment.storeOp = (VkAttachmentStoreOp)info->colorAttachmentStoreOp;

	VkClearValue clearValue{};
	clearValue.color = { {info->colorAttachment->clearValue.r, info->colorAttachment->clearValue.g,
		info->colorAttachment->clearValue.b, info->colorAttachment->clearValue.a} };
	colorAttachment.clearValue = clearValue;

	VkRenderingAttachmentInfoKHR depthAttachment{};
	if (info->depthAttachment)
	{
		depthAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
		depthAttachment.pNext = nullptr;
		depthAttachment.imageView = info->depthAttachment->imageView;
		depthAttachment.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		depthAttachment.resolveMode = VK_RESOLVE_MODE_NONE;
		depthAttachment.loadOp = (VkAttachmentLoadOp)info->depthAttachmentLoadOp;
		depthAttachment.storeOp = (VkAttachmentStoreOp)info->depthAttachmentStoreOp;

		VkClearValue clearValue{};
		clearValue.depthStencil = {info->depthAttachment->clearValue.depth, info->depthAttachment->clearValue.stencil};
		depthAttachment.clearValue = clearValue;
	}

	VkRenderingInfoKHR renderingInfo{};
	renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR;
	renderingInfo.pNext = nullptr;
	renderingInfo.flags = 0;
	renderingInfo.renderArea.offset = { 0, 0 };
	renderingInfo.renderArea.extent = { info->colorAttachment->width, info->colorAttachment->height };
	renderingInfo.layerCount = 1;
	renderingInfo.viewMask = 0;
	renderingInfo.colorAttachmentCount = 1;
	renderingInfo.pColorAttachments = &colorAttachment;
	if (info->depthAttachment)
		renderingInfo.pDepthAttachment = &depthAttachment;

	vkCmdBeginRendering(commandBuffer->commandBuffer, &renderingInfo);
	commandBuffer->isRendering = true;
}

void VulkanBindPipeline(const VulkanCommandBuffer* const commandBuffer, const VulkanPipeline* const pipeline)
{
	switch (pipeline->type)
	{
	case PIPELINE_TYPE_GRAPHICS:
		vkCmdBindPipeline(commandBuffer->commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->pipeline);
		break;

	case PIPELINE_TYPE_COMPUTE:
		vkCmdBindPipeline(commandBuffer->commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline->pipeline);
		break;
	}
}

void VulkanSetViewport(const VulkanCommandBuffer* const commandBuffer, const ViewportInfo* const viewportInfo)
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

void VulkanSetScissor(const VulkanCommandBuffer* const commandBuffer, const ScissorInfo* const scissorInfo)
{
	VkRect2D scissor{};
	scissor.offset.x = scissorInfo->x;
	scissor.offset.y = scissorInfo->y;
	scissor.extent.width = scissorInfo->width;
	scissor.extent.height = scissorInfo->height;
	vkCmdSetScissor(commandBuffer->commandBuffer, 0, 1, &scissor);
}

void VulkanDraw(const VulkanCommandBuffer* const commandBuffer, const uint32_t vertexCount,
	const uint32_t instanceCount, const  uint32_t const firstVertex, const uint32_t firstInstance)
{
	vkCmdDraw(commandBuffer->commandBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
}

void VulkanDrawIndexed(const VulkanCommandBuffer* const commandBuffer, const uint32_t indexCount,
	const uint32_t instanceCount, const uint32_t firstIndex, const uint32_t vertexOffset, const uint32_t firstInstance)
{
	vkCmdDrawIndexed(commandBuffer->commandBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}

void VulkanDispatch(const VulkanCommandBuffer* const commandBuffer, const uint32_t x, const uint32_t y, const uint32_t z)
{
	vkCmdDispatch(commandBuffer->commandBuffer, x, y, z);
}

void VulkanEndCommandBuffer(const VulkanCommandBuffer* const commandBuffer)
{
	VULKAN_ERROR_CHECK(vkEndCommandBuffer(commandBuffer->commandBuffer));
}
//-------------------------------------------------------------------------------------------------------------------------------------------

//SYNCHRONIZATION
//-------------------------------------------------------------------------------------------------------------------------------------------
void VulkanCreateFence(const Vulkan* const vulk, VulkanFence* fence)
{
	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.pNext = nullptr;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
	
	VULKAN_ERROR_CHECK(vkCreateFence(vulk->logicalDevice, &fenceInfo, nullptr, &fence->fence));
}

void VulkanDestroyFence(const Vulkan* const vulk, VulkanFence* fence)
{
	vkDestroyFence(vulk->logicalDevice, fence->fence, nullptr);
}

void VulkanWaitForFence(const Vulkan* const vulk, const VulkanFence* const fence)
{
	vkWaitForFences(vulk->logicalDevice, 1, &fence->fence, VK_TRUE, UINT64_MAX);
	vkResetFences(vulk->logicalDevice, 1, &fence->fence);
}

void VulkanCreateSemaphore(const Vulkan* const vulk, VulkanSemaphore* semaphore)
{
	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	semaphoreInfo.pNext = nullptr;
	semaphoreInfo.flags = 0;

	VULKAN_ERROR_CHECK(vkCreateSemaphore(vulk->logicalDevice, &semaphoreInfo, nullptr, &semaphore->semaphore));
}

void VulkanDestroySemaphore(const Vulkan* const vulk, VulkanSemaphore* semaphore)
{
	vkDestroySemaphore(vulk->logicalDevice, semaphore->semaphore, nullptr);
}

//-------------------------------------------------------------------------------------------------------------------------------------------

void VulkanAcquireNextImage(const Vulkan* const vulk, 
	const VulkanSwapChain* const swapChain, const VulkanSemaphore* const imageAvailableSemaphore, uint32_t* imageIndex)
{
	vkAcquireNextImageKHR(vulk->logicalDevice, swapChain->swapChain, UINT64_MAX, imageAvailableSemaphore->semaphore,
		nullptr, imageIndex);
}

//SUBMIT AND PRESENT
//-------------------------------------------------------------------------------------------------------------------------------------------
void VulkanQueueSubmit(Vulkan* vulk, VulkanQueueSubmitInfo* info)
{
	VkSemaphore* waitSemaphores = nullptr;
	VkPipelineStageFlags* waitStages = nullptr;
	if (info->waitSemaphores)
	{
		waitSemaphores = (VkSemaphore*)calloc(info->numWaitSemaphores, sizeof(VkSemaphore));
		waitStages = (VkPipelineStageFlags*)calloc(info->numWaitSemaphores, sizeof(VkPipelineStageFlags));
		for (uint32_t i = 0; i < info->numWaitSemaphores; ++i)
		{
			waitSemaphores[i] = info->waitSemaphores[i].semaphore;
			waitStages[i] = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
		}
	}


	VkSemaphore* signalSemaphores = nullptr;
	if (info->signalSemaphores)
	{
		signalSemaphores = (VkSemaphore*)calloc(info->numSignalSemaphores, sizeof(VkSemaphore));

		for (uint32_t i = 0; i < info->numSignalSemaphores; ++i)
		{
			signalSemaphores[i] = info->signalSemaphores[i].semaphore;
		}
	}

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.pNext = nullptr;
	submitInfo.waitSemaphoreCount = info->numWaitSemaphores;
	submitInfo.pWaitSemaphores = waitSemaphores;

	if (!waitStages)
		submitInfo.pWaitDstStageMask = nullptr;
	else
		submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &info->commandBuffer->commandBuffer;
	submitInfo.signalSemaphoreCount = info->numSignalSemaphores;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (info->fence)
	{
		VULKAN_ERROR_CHECK(vkQueueSubmit(info->queue->queue, 1, &submitInfo, info->fence->fence));
	}
	else
	{
		VULKAN_ERROR_CHECK(vkQueueSubmit(info->queue->queue, 1, &submitInfo, VK_NULL_HANDLE));
	}


	if(waitSemaphores)
		free(waitSemaphores);

	if (waitStages)
		free(waitStages);

	if(signalSemaphores)
		free(signalSemaphores);
}

void VulkanQueuePresent(Vulkan* vulk, VulkanPresentInfo* info)
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

void VulkanWaitDeviceIdle(Vulkan* vulk)
{
	vkDeviceWaitIdle(vulk->logicalDevice);
}

void VulkanWaitQueueIdle(VulkanQueue* queue)
{
	vkQueueWaitIdle(queue->queue);
}
//-------------------------------------------------------------------------------------------------------------------------------------------

//RESIZE
//-------------------------------------------------------------------------------------------------------------------------------------------
void VulkanOnResize(Vulkan* vulk, VulkanSwapChain* swapChain, VulkanRenderAttachment* depthBuffer)
{
	if (vulk->instance == nullptr)
		return;

	VulkanWaitDeviceIdle(vulk);

	if(depthBuffer)
		VulkanDestroyRenderingAttachment(vulk, depthBuffer);

	VulkanDestroySwapChain(vulk, swapChain);

	VulkanSwapChainInfo swapChainInfo{};
	swapChainInfo.window = swapChain->window;
	swapChainInfo.width = GetWidth(swapChain->window);
	swapChainInfo.height = GetHeight(swapChain->window);
	swapChainInfo.format = swapChain->format;
	VulkanCreateSwapChain(vulk, &swapChainInfo, swapChain);

	if (depthBuffer)
	{
		VulkanRenderAttachmentInfo dbInfo{};
		dbInfo.format = depthBuffer->format;
		dbInfo.width = swapChain->width;
		dbInfo.height = swapChain->height;
		dbInfo.clearValue = depthBuffer->clearValue;
		VulkanCreateRenderingAttachment(vulk, &dbInfo, depthBuffer);
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------------

//BUFFERS
//-------------------------------------------------------------------------------------------------------------------------------------------
void VulkanCreateStagingBuffer(const Vulkan* const vulk, const VkDeviceSize size)
{
	VkBufferCreateInfo bufferCreateInfo{};
	bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferCreateInfo.pNext = nullptr;
	bufferCreateInfo.flags = 0;
	bufferCreateInfo.size = size;
	bufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	bufferCreateInfo.queueFamilyIndexCount = 0;
	bufferCreateInfo.pQueueFamilyIndices = nullptr;

	VmaAllocationCreateInfo allocCreateInfo = {};
	allocCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
	allocCreateInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
	allocCreateInfo.priority = 1.0f;

	VULKAN_ERROR_CHECK(vmaCreateBuffer(vulk->allocator, &bufferCreateInfo, &allocCreateInfo, &gVulkanCopyEngine.stagingBuffer,
		&gVulkanCopyEngine.stagingBufferAllocation, nullptr));
}

void VulkanDestroyStagingBuffer(const Vulkan* const vulk)
{
	vmaDestroyBuffer(vulk->allocator, gVulkanCopyEngine.stagingBuffer, gVulkanCopyEngine.stagingBufferAllocation);
}

void VulkanCopyBuffer(const Vulkan* const vulk, void* srcData, VkBuffer* dstBuffer, VkDeviceSize size)
{
	VulkanCreateStagingBuffer(vulk, size);
	void* data = nullptr;
	vmaMapMemory(vulk->allocator, gVulkanCopyEngine.stagingBufferAllocation, &data);
	memcpy(data, srcData, size);
	vmaUnmapMemory(vulk->allocator, gVulkanCopyEngine.stagingBufferAllocation);

	VulkanBeginCopyEngineCommandBuffer();

	VkBufferCopy copyRegion{};
	copyRegion.srcOffset = 0;
	copyRegion.dstOffset = 0;
	copyRegion.size = size;
	vkCmdCopyBuffer(gVulkanCopyEngine.commandBuffer, gVulkanCopyEngine.stagingBuffer, *dstBuffer, 1, &copyRegion);

	VulkanEndCopyEngineCommandBuffer();

	VulkanDestroyStagingBuffer(vulk);
}

void VulkanCreateBuffer(const Vulkan* const vulk, const VulkanBufferInfo* const bufferInfo, VulkanBuffer* buffer)
{
	buffer->type = bufferInfo->type;

	bool copyData = false;
	if (bufferInfo->data && bufferInfo->access == ACCESS_GPU)
	{
		copyData = true;
	}

	VkBufferCreateInfo bufferCreateInfo{};
	bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferCreateInfo.pNext = nullptr;
	bufferCreateInfo.flags = 0;
	bufferCreateInfo.size = bufferInfo->size;

	if (bufferInfo->type & BUFFER_TYPE_VERTEX_BUFFER)
		bufferCreateInfo.usage |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

	if (bufferInfo->type & BUFFER_TYPE_INDEX_BUFFER)
	{
		bufferCreateInfo.usage |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
		buffer->indexType = bufferInfo->indexType;
	}

	if (bufferInfo->type & BUFFER_TYPE_UNIFORM_BUFFER)
		bufferCreateInfo.usage |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

	if (bufferInfo->type & BUFFER_TYPE_STORAGE_BUFFER)
		bufferCreateInfo.usage |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;

	if(copyData)
		bufferCreateInfo.usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;

	bufferCreateInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
	bufferCreateInfo.queueFamilyIndexCount = 3;
	bufferCreateInfo.pQueueFamilyIndices = vulk->familyIndices;

	VmaAllocationCreateInfo allocationInfo{};
	allocationInfo.usage = VMA_MEMORY_USAGE_AUTO;

	switch (bufferInfo->access)
	{
	case ACCESS_GPU:
		allocationInfo.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
		break;

	case ACCESS_CPU_GPU:
		allocationInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
		break;
	}

	allocationInfo.priority = 1.0f;

	VULKAN_ERROR_CHECK(vmaCreateBuffer(vulk->allocator, &bufferCreateInfo, &allocationInfo, &buffer->buffer,
		&buffer->allocation, nullptr));

	if (copyData)
		VulkanCopyBuffer(vulk, bufferInfo->data, &buffer->buffer, bufferInfo->size);
}

void VulkanDestroyBuffer(const Vulkan* const vulk, VulkanBuffer* buffer)
{
	vmaDestroyBuffer(vulk->allocator, buffer->buffer, buffer->allocation);
}

void VulkanMapMemory(const Vulkan* const vulk, const VulkanBuffer* const buffer, void** data)
{
	VULKAN_ERROR_CHECK(vmaMapMemory(vulk->allocator, buffer->allocation, data));
}

void VulkanUnmapMemory(const Vulkan* const vulk, const VulkanBuffer* const buffer)
{
	vmaUnmapMemory(vulk->allocator, buffer->allocation);
}

void VulkanBindBuffer(const VulkanCommandBuffer* const commandBuffer, const uint32_t bindingLocation,
	const uint32_t offset, const VulkanBuffer* const buffer)
{
	VkDeviceSize offs = offset;

	if (buffer->type & BUFFER_TYPE_VERTEX_BUFFER)
	{
		vkCmdBindVertexBuffers(commandBuffer->commandBuffer, bindingLocation, 1, &buffer->buffer, &offs);
	}
	else if (buffer->type & BUFFER_TYPE_INDEX_BUFFER)
	{
		vkCmdBindIndexBuffer(commandBuffer->commandBuffer, buffer->buffer, offs, (VkIndexType)buffer->indexType);
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------------

//DESCRIPTORS
//-------------------------------------------------------------------------------------------------------------------------------------------
void VulkanCreateDescriptorSet(const Vulkan* const vulk, const VulkanDescriptorSetInfo* const info, VulkanDescriptorSet* ds)
{
	ds->descriptorSets = (VkDescriptorSet*)calloc(info->numDescriptorSets, sizeof(VkDescriptorSet));
	VkDescriptorSetLayout* layouts = (VkDescriptorSetLayout*)calloc(info->numDescriptorSets, sizeof(VkDescriptorSetLayout));

	for (uint32_t i = 0; i < info->numDescriptorSets; ++i)
	{
		layouts[i] = info->layouts[i].descriptorSetLayout;
	}

	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.pNext = nullptr;
	allocInfo.descriptorPool = gDescriptorPool;
	allocInfo.descriptorSetCount = info->numDescriptorSets;
	allocInfo.pSetLayouts = layouts;

	VULKAN_ERROR_CHECK(vkAllocateDescriptorSets(vulk->logicalDevice, &allocInfo, ds->descriptorSets));
	free(layouts);
}

void VulkanDestroyDescriptorSet(VulkanDescriptorSet* ds)
{
	free(ds->descriptorSets);
}

void VulkanUpdateDescriptorSet(const Vulkan* const vulk, const VulkanUpdateDescriptorSetInfo* const info, 
	const VulkanDescriptorSet* const ds)
{
	if (info->bufferInfo.buffer)
	{
		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = info->bufferInfo.buffer->buffer;
		bufferInfo.offset = info->bufferInfo.offset;
		bufferInfo.range = info->bufferInfo.size;

		VkWriteDescriptorSet descriptorWrite{};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.pNext = nullptr;
		descriptorWrite.dstSet = ds->descriptorSets[info->index];
		descriptorWrite.dstBinding = info->binding;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorCount = 1;

		switch (info->bufferInfo.type)
		{
		case DESCRITPTOR_TYPE_UNIFORM_BUFFER:
			descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			break;

		case DESCRITPTOR_TYPE_STORAGE_BUFFER:
			descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			break;

		}
		descriptorWrite.pBufferInfo = &bufferInfo;

		vkUpdateDescriptorSets(vulk->logicalDevice, 1, &descriptorWrite, 0, nullptr);
	}
	else if (info->imageInfo.texture)
	{
		VkDescriptorImageInfo imageInfo{};
		imageInfo.sampler = info->imageInfo.sampler->sampler;
		imageInfo.imageView = info->imageInfo.texture->imageView;
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		VkWriteDescriptorSet descriptorWrite{};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.pNext = nullptr;
		descriptorWrite.dstSet = ds->descriptorSets[info->index];
		descriptorWrite.dstBinding = info->binding;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWrite.pImageInfo = &imageInfo;

		vkUpdateDescriptorSets(vulk->logicalDevice, 1, &descriptorWrite, 0, nullptr);
	}
}

void VulkanBindDescriptorSet(const VulkanCommandBuffer* const commandBuffer, 
	const VulkanPipeline* const pipeline, const uint32_t index, const VulkanDescriptorSet* const ds)
{
	VkPipelineBindPoint bindPoint{};
	switch (pipeline->type)
	{
	case PIPELINE_TYPE_GRAPHICS:
		bindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		break;

	case PIPELINE_TYPE_COMPUTE:
		bindPoint = VK_PIPELINE_BIND_POINT_COMPUTE;
		break;
	}

	vkCmdBindDescriptorSets(commandBuffer->commandBuffer, bindPoint, pipeline->pipelineLayout, 0, 1, &ds->descriptorSets[index], 0, nullptr);
}
//-------------------------------------------------------------------------------------------------------------------------------------------

//TEXTURES AND SAMPLERS
//-------------------------------------------------------------------------------------------------------------------------------------------
void VulkanTransitionImage(const VulkanCommandBuffer* const commandBuffer, const VulkanRenderAttachment* const attachment,
	const VulkanImageLayout oldLayout, const VulkanImageLayout newLayout)
{
	bool const isDepth = TinyImageFormat_IsDepthOnly(attachment->format) || TinyImageFormat_IsDepthAndStencil(attachment->format);

	VkImageMemoryBarrier barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.pNext = nullptr;
	barrier.srcAccessMask = 0;
	barrier.dstAccessMask = 0;
	barrier.oldLayout = (VkImageLayout)oldLayout;
	barrier.newLayout = (VkImageLayout)newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = attachment->image;

	if(isDepth)
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
	else
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;

	VkPipelineStageFlagBits srcFlags{};
	VkPipelineStageFlagBits dstFlags{};

	switch (oldLayout) 
	{
	case VK_IMAGE_LAYOUT_UNDEFINED:
		barrier.srcAccessMask = 0;
		srcFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		break;

	case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
		barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		srcFlags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		break;

	case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
		barrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		srcFlags = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		break;

	case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		srcFlags = VK_PIPELINE_STAGE_TRANSFER_BIT;
		break;

	case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
		barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
		srcFlags = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
		break;
	}

	switch (newLayout) 
	{
	case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		dstFlags = VK_PIPELINE_STAGE_TRANSFER_BIT;
		break;

	case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
		barrier.srcAccessMask |= VK_ACCESS_TRANSFER_READ_BIT;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		dstFlags = VK_PIPELINE_STAGE_TRANSFER_BIT;
		break;

	case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
		barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		dstFlags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		break;

	case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
		barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		dstFlags = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		break;

	case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		dstFlags = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
		break;

	case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
		barrier.dstAccessMask = 0;
		dstFlags = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
	}

	vkCmdPipelineBarrier(commandBuffer->commandBuffer, srcFlags, dstFlags, 0, 0, NULL, 0, NULL, 1, &barrier);
}

void VulkanTransitionImage(const TextureInfo* const textureInfo, const VulkanTexture* const texture,
	const VkImageLayout oldLayout, const VkImageLayout newLayout)
{
	VkImageMemoryBarrier barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.pNext = nullptr;
	barrier.srcAccessMask = 0;
	barrier.dstAccessMask = 0;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = texture->image;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = textureInfo->mipCount;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = textureInfo->arraySize;

	VkPipelineStageFlags sourceStage;
	VkPipelineStageFlags destinationStage;

	switch (oldLayout)
	{
	case VK_IMAGE_LAYOUT_UNDEFINED:
		barrier.srcAccessMask = 0;
		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		break;

	case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		break;
	}

	switch (newLayout)
	{
	case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		break;

	case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		destinationStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
		break;
	}

	vkCmdPipelineBarrier(gVulkanCopyEngine.commandBuffer, sourceStage, destinationStage, 0, 0, NULL, 0, NULL, 1, &barrier);
}

void VulkanCopyBufferToImage(const TextureInfo* const textureInfo, const VulkanTexture* const texture)
{
	VkBufferImageCopy region{};
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;
	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = textureInfo->arraySize;
	region.imageOffset = { 0, 0, 0 };
	region.imageExtent = { textureInfo->width, textureInfo->height, textureInfo->depth };

	vkCmdCopyBufferToImage(gVulkanCopyEngine.commandBuffer, gVulkanCopyEngine.stagingBuffer, texture->image,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
}

void VulkanCreateTexture(const Vulkan* const vulk, const char* filename, VulkanTexture* texture)
{
	uint8_t* bitData = nullptr;
	uint32_t numBytes = 0;
	DDS_HEADER ddsHeader{};
	DDS_HEADER_DXT10 ddsHeader10{};
	ReadDDSFile(filename, &bitData, &numBytes, &ddsHeader, &ddsHeader10);

	TextureInfo texInfo{};
	int result = RetrieveTextureInfo(&ddsHeader, &ddsHeader10, bitData, numBytes, &texInfo);
	if (result != SE_SUCCESS)
	{
		MessageBox(nullptr, L"Error with function RetrieveTextureInfo in function VulkanCreateTexture. Exiting Program.", 
			L"RetrieveTextureInfo Error", MB_OK);
		free(bitData);
		exit(5);
	}

	VulkanCreateStagingBuffer(vulk, numBytes);

	void* data = nullptr;
	VULKAN_ERROR_CHECK(vmaMapMemory(vulk->allocator, gVulkanCopyEngine.stagingBufferAllocation, &data));

	uint32_t numImages = texInfo.arraySize * texInfo.mipCount;
	uint32_t offset = 0;
	uint8_t* srcData = (uint8_t*)data;
	for (uint32_t i = 0; i < numImages; ++i)
	{
		memcpy(srcData + offset, texInfo.images[i].data, texInfo.images[i].numBytes);
		offset += texInfo.images[i].numBytes;
	}

	vmaUnmapMemory(vulk->allocator, gVulkanCopyEngine.stagingBufferAllocation);

	free(bitData);
	free(texInfo.images);

	VkImageCreateInfo createImageInfo{};
	createImageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	createImageInfo.pNext = nullptr;
	createImageInfo.flags = 0;

	switch (texInfo.resDim)
	{
	case SE_TEXTURE_1D:
		createImageInfo.imageType = VK_IMAGE_TYPE_1D;
		break;
	case SE_TEXTURE_2D:
		createImageInfo.imageType = VK_IMAGE_TYPE_2D;
		break;
	case SE_TEXTURE_3D:
		createImageInfo.imageType = VK_IMAGE_TYPE_3D;
		break;
	}

	createImageInfo.format = (VkFormat)TinyImageFormat_ToVkFormat(texInfo.format);
	createImageInfo.extent.width = texInfo.width;
	createImageInfo.extent.height = texInfo.height;
	createImageInfo.extent.depth = texInfo.depth;
	createImageInfo.mipLevels = texInfo.mipCount;
	createImageInfo.arrayLayers = texInfo.arraySize;
	createImageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	createImageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	createImageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	createImageInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
	createImageInfo.queueFamilyIndexCount = 3;
	createImageInfo.pQueueFamilyIndices = vulk->familyIndices;
	createImageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

	VmaAllocationCreateInfo allocCreateInfo = {};
	allocCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
	allocCreateInfo.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
	allocCreateInfo.priority = 1.0f;

	VULKAN_ERROR_CHECK(vmaCreateImage(vulk->allocator, &createImageInfo, &allocCreateInfo, &texture->image, &texture->allocation, nullptr));

	VulkanBeginCopyEngineCommandBuffer();

	VulkanTransitionImage(&texInfo, texture, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	VulkanCopyBufferToImage(&texInfo, texture);

	VulkanTransitionImage(&texInfo, texture, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	VulkanEndCopyEngineCommandBuffer();

	VulkanDestroyStagingBuffer(vulk);

	VkImageViewCreateInfo createImageViewInfo{};
	createImageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	createImageViewInfo.pNext = nullptr;
	createImageViewInfo.flags = 0;
	createImageViewInfo.image = texture->image;
	if (texInfo.resDim == SE_TEXTURE_1D)
	{
		if (texInfo.arraySize > 1)
		{
			createImageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_1D_ARRAY;
		}
		else
		{
			createImageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_1D;
		}
	}
	else if (texInfo.resDim == SE_TEXTURE_2D)
	{
		if (texInfo.isCubeMap)
		{
			createImageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_CUBE;
		}
		else if (texInfo.arraySize > 1)
		{
			createImageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
		}
		else
		{
			createImageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		}
	}
	else if (texInfo.resDim == SE_TEXTURE_3D)
	{
		createImageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_3D;
	}
	createImageViewInfo.format = (VkFormat)TinyImageFormat_ToVkFormat(texInfo.format);
	createImageViewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	createImageViewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	createImageViewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	createImageViewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
	createImageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	createImageViewInfo.subresourceRange.baseMipLevel = 0;
	createImageViewInfo.subresourceRange.levelCount = 1;
	createImageViewInfo.subresourceRange.baseArrayLayer = 0;
	createImageViewInfo.subresourceRange.layerCount = 1;

	VULKAN_ERROR_CHECK(vkCreateImageView(vulk->logicalDevice, &createImageViewInfo, nullptr, &texture->imageView));
}

void VulkanDestroyTexture(const Vulkan* const vulk, VulkanTexture* texture)
{
	vkDestroyImageView(vulk->logicalDevice, texture->imageView, nullptr);
	vmaDestroyImage(vulk->allocator, texture->image, texture->allocation);
}

void VulkanCreateSampler(const Vulkan* const vulk, const VulkanSamplerInfo* const info, VulkanSampler* sampler)
{
	VkSamplerCreateInfo samplerInfo{};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.pNext = nullptr;
	samplerInfo.flags = 0;

	switch (info->magFilter)
	{
	case NEAREST_FILTER:
		samplerInfo.magFilter = VK_FILTER_NEAREST;
		break;
	case LINEAR_FILTER:
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		break;
	}

	switch (info->minFilter)
	{
	case NEAREST_FILTER:
		samplerInfo.minFilter = VK_FILTER_NEAREST;
		break;
	case LINEAR_FILTER:
		samplerInfo.minFilter = VK_FILTER_LINEAR;
		break;
	}

	switch (info->u)
	{
	case ADDRESS_MODE_REPEAT:
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		break;

	case ADDRESS_MODE_MIRRORED_REPEAT:
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
		break;

	case ADDRESS_MODE_CLAMP_TO_EDGE:
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		break;

	case ADDRESS_MODE_CLAMP_TO_BORDER:
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
		break;
	}

	switch (info->v)
	{
	case ADDRESS_MODE_REPEAT:
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		break;

	case ADDRESS_MODE_MIRRORED_REPEAT:
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
		break;

	case ADDRESS_MODE_CLAMP_TO_EDGE:
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		break;

	case ADDRESS_MODE_CLAMP_TO_BORDER:
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
		break;
	}

	switch (info->w)
	{
	case ADDRESS_MODE_REPEAT:
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		break;

	case ADDRESS_MODE_MIRRORED_REPEAT:
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
		break;

	case ADDRESS_MODE_CLAMP_TO_EDGE:
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		break;

	case ADDRESS_MODE_CLAMP_TO_BORDER:
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
		break;
	}

	samplerInfo.anisotropyEnable = VK_TRUE;
	samplerInfo.maxAnisotropy = info->maxAnisotropy > gMaxSamplerAnisotropy ? gMaxSamplerAnisotropy : info->maxAnisotropy;
	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;

	switch (info->mipMapMode)
	{
	case MIPMAP_MODE_NEAREST:
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
		break;

	case MIPMAP_MODE_LINEAR:
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		break;
	}
	samplerInfo.mipLodBias = info->mipLoadBias;
	samplerInfo.minLod = info->minLod;
	samplerInfo.maxLod = info->maxLod;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;

	VULKAN_ERROR_CHECK(vkCreateSampler(vulk->logicalDevice, &samplerInfo, nullptr, &sampler->sampler));
}

void VulkanDestroySampler(const Vulkan* const vulk, VulkanSampler* sampler)
{
	vkDestroySampler(vulk->logicalDevice, sampler->sampler, nullptr);
}
//-------------------------------------------------------------------------------------------------------------------------------------------