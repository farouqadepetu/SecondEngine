#define VOLK_IMPLEMENTATION
#define VMA_IMPLEMENTATION
#include "../SERenderer.h"


#define GRAPHICS_FAMILY 0
#define COMPUTE_FAMILY 1
#define TRANSFER_FAMILY 2

#ifndef VULKAN_ERROR_CHECK
#define VULKAN_ERROR_CHECK(x)																								\
{																															\
	VkResult result = (x);																									\
	if (result != VK_SUCCESS)																								\
	{																														\
		char errMsg[2048]{};																								\
		snprintf(errMsg, 2048, "%s\nFailed in %s\nLine %d\nError = %s", #x, __FILE__, __LINE__, string_VkResult(result));		\
		MessageBoxA(nullptr, errMsg, "Vulkan Function Failed", MB_OK);														\
		exit(2);																											\
	}																														\
}																																
#endif

//DEBUG 
//-------------------------------------------------------------------------------------------------------------------------------------------
static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData) 
{
	
	OutputDebugStringA(pCallbackData->pMessage);
	OutputDebugStringA("\n");

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

void SetupDebugMessenger(Renderer* pRenderer)
{
	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
	PopulateDebugMessengerCreateInfo(&debugCreateInfo);

	VULKAN_ERROR_CHECK(CreateDebugUtilsMessengerEXT(pRenderer->vk.instance, &debugCreateInfo, nullptr, &pRenderer->vk.debugMessenger));
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

bool CheckDeviceExtensionSupport(const Renderer* pRenderer, const char** extensionNames, uint32_t numExtensions)
{
	uint32_t extensionCount = 0;
	vkEnumerateDeviceExtensionProperties(pRenderer->vk.physicalDevice, nullptr, &extensionCount, nullptr);

	VkExtensionProperties* extensions = (VkExtensionProperties*)calloc(extensionCount, sizeof(VkExtensionProperties));
	vkEnumerateDeviceExtensionProperties(pRenderer->vk.physicalDevice, nullptr, &extensionCount, extensions);

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

//SUBMIT AND PRESENT
//-------------------------------------------------------------------------------------------------------------------------------------------
#define MAX_NUM_SEMAPHORES 8
void VulkanQueueSubmit(const QueueSubmitInfo* const pInfo)
{
	VkSemaphore waitSemaphores[MAX_NUM_SEMAPHORES]{};
	VkPipelineStageFlags waitStages[MAX_NUM_SEMAPHORES]{};
	if (pInfo->waitSemaphores)
	{
		for (uint32_t i = 0; i < pInfo->numWaitSemaphores; ++i)
		{
			waitSemaphores[i] = pInfo->waitSemaphores[i].vk.semaphore;
			waitStages[i] = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
		}
	}

	VkSemaphore signalSemaphores[MAX_NUM_SEMAPHORES]{};
	if (pInfo->signalSemaphores)
	{
		for (uint32_t i = 0; i < pInfo->numSignalSemaphores; ++i)
		{
			signalSemaphores[i] = pInfo->signalSemaphores[i].vk.semaphore;
		}
	}

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.pNext = nullptr;
	submitInfo.waitSemaphoreCount = pInfo->numWaitSemaphores;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = (waitStages == nullptr) ? nullptr : waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &pInfo->pCommandBuffer->vk.commandBuffer;
	submitInfo.signalSemaphoreCount = pInfo->numSignalSemaphores;
	submitInfo.pSignalSemaphores = signalSemaphores;

	VULKAN_ERROR_CHECK(vkQueueSubmit(pInfo->pQueue->vk.queue, 1, &submitInfo, pInfo->pFence ? pInfo->pFence->vk.fence : nullptr));
}

void VulkanQueuePresent(const PresentInfo* const pInfo)
{
	VkSemaphore waitSemaphores[MAX_NUM_SEMAPHORES]{};
	for (uint32_t i = 0; i < pInfo->numWaitSemaphores; ++i)
	{
		waitSemaphores[i] = pInfo->waitSemaphores->vk.semaphore;
	}

	VkSwapchainKHR swapChains[] = { pInfo->pSwapChain->vk.swapChain };

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.pNext = nullptr;
	presentInfo.waitSemaphoreCount = pInfo->numWaitSemaphores;
	presentInfo.pWaitSemaphores = waitSemaphores;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &pInfo->imageIndex;
	presentInfo.pResults = nullptr;

	VULKAN_ERROR_CHECK(vkQueuePresentKHR(pInfo->pQueue->vk.queue, &presentInfo));
}

void VulkanWaitQueueIdle(const Renderer* const pRenderer, Queue* pQueue)
{
	vkQueueWaitIdle(pQueue->vk.queue);
}
//-------------------------------------------------------------------------------------------------------------------------------------------

//SYNCHRONIZATION
//-------------------------------------------------------------------------------------------------------------------------------------------
void VulkanCreateFence(const Renderer* const pRenderer, Fence* pFence)
{
	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.pNext = nullptr;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	VULKAN_ERROR_CHECK(vkCreateFence(pRenderer->vk.logicalDevice, &fenceInfo, nullptr, &pFence->vk.fence));
}

void VulkanDestroyFence(const Renderer* const pRenderer, Fence* pFence)
{
	vkDestroyFence(pRenderer->vk.logicalDevice, pFence->vk.fence, nullptr);
}

void VulkanWaitForFence(const Renderer* const pRenderer, Fence* pFence)
{
	vkWaitForFences(pRenderer->vk.logicalDevice, 1, &pFence->vk.fence, VK_TRUE, UINT64_MAX);
	vkResetFences(pRenderer->vk.logicalDevice, 1, &pFence->vk.fence);
}

void VulkanCreateSemaphore(const Renderer* const pRenderer, Semaphore* pSemaphore)
{
	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	semaphoreInfo.pNext = nullptr;
	semaphoreInfo.flags = 0;

	VULKAN_ERROR_CHECK(vkCreateSemaphore(pRenderer->vk.logicalDevice, &semaphoreInfo, nullptr, &pSemaphore->vk.semaphore));
}

void VulkanDestroySemaphore(const Renderer* const pRenderer, Semaphore* pSemaphore)
{
	vkDestroySemaphore(pRenderer->vk.logicalDevice, pSemaphore->vk.semaphore, nullptr);
}

//-------------------------------------------------------------------------------------------------------------------------------------------

void VulkanAcquireNextImage(const Renderer* const pRenderer,
	const SwapChain* const pSwapChain, const Semaphore* const pSemaphore, uint32_t* pImageIndex)
{
	vkAcquireNextImageKHR(pRenderer->vk.logicalDevice, pSwapChain->vk.swapChain, UINT64_MAX, pSemaphore->vk.semaphore,
		nullptr, pImageIndex);
}

//COMMAND BUFFERS
//-------------------------------------------------------------------------------------------------------------------------------------------
void VulkanCreateCommandBuffer(const Renderer* const pRenderer, const QueueType type, CommandBuffer* pCommandBuffer)
{
	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.pNext = nullptr;
	poolInfo.flags = 0;
	switch (type)
	{
	case QUEUE_TYPE_GRAPHICS:
		poolInfo.queueFamilyIndex = pRenderer->vk.familyIndices[GRAPHICS_FAMILY];
		break;

	case QUEUE_TYPE_COMPUTE:
		poolInfo.queueFamilyIndex = pRenderer->vk.familyIndices[COMPUTE_FAMILY];
		break;

	case QUEUE_TYPE_COPY:
		poolInfo.queueFamilyIndex = pRenderer->vk.familyIndices[TRANSFER_FAMILY];
		break;
	}


	VULKAN_ERROR_CHECK(vkCreateCommandPool(pRenderer->vk.logicalDevice, &poolInfo, nullptr, &pCommandBuffer->vk.commandPool));

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.pNext = nullptr;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 1;
	allocInfo.commandPool = pCommandBuffer->vk.commandPool;

	VULKAN_ERROR_CHECK(vkAllocateCommandBuffers(pRenderer->vk.logicalDevice, &allocInfo, &pCommandBuffer->vk.commandBuffer));

	VulkanCreateFence(pRenderer, &pCommandBuffer->fence);
	VulkanCreateSemaphore(pRenderer, &pCommandBuffer->semaphore);

	pCommandBuffer->type = type;
}

void VulkanDestroyCommandBuffer(const Renderer* const pRenderer, CommandBuffer* pCommandBuffer)
{
	vkDestroyCommandPool(pRenderer->vk.logicalDevice, pCommandBuffer->vk.commandPool, nullptr);

	VulkanDestroySemaphore(pRenderer, &pCommandBuffer->semaphore);
	VulkanDestroyFence(pRenderer, &pCommandBuffer->fence);
}

void VulkanResetCommandBuffer(const Renderer* const pRenderer, const CommandBuffer* const pCommandBuffer)
{
	VULKAN_ERROR_CHECK(vkResetCommandPool(pRenderer->vk.logicalDevice, pCommandBuffer->vk.commandPool, 0));
}

void VulkanBeginCommandBuffer(const CommandBuffer* const pCommandBuffer)
{
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.pNext = nullptr;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	beginInfo.pInheritanceInfo = nullptr;

	VULKAN_ERROR_CHECK(vkBeginCommandBuffer(pCommandBuffer->vk.commandBuffer, &beginInfo));
}

void VulkanEndCommandBuffer(const CommandBuffer* const pCommandBuffer)
{
	VULKAN_ERROR_CHECK(vkEndCommandBuffer(pCommandBuffer->vk.commandBuffer));
}
//-------------------------------------------------------------------------------------------------------------------------------------------

VkImageLayout VulkanResourceStateToImageLayout(const ResourceState state)
{
	if (state & RESOURCE_STATE_COPY_SOURCE)
		return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;

	if (state & RESOURCE_STATE_COPY_DEST)
		return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;

	if (state & RESOURCE_STATE_RENDER_TARGET)
		return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	if (state & RESOURCE_STATE_DEPTH_WRITE)
		return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	else if (state & RESOURCE_STATE_DEPTH_READ)
		return VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;

	if (state & RESOURCE_STATE_UNORDERED_ACCESS)
		return VK_IMAGE_LAYOUT_GENERAL;

	if (state & RESOURCE_STATE_ALL_SHADER_RESOURCE)
		return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	if (state & RESOURCE_STATE_PRESENT)
		return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	if (state == RESOURCE_STATE_COMMON)
		return VK_IMAGE_LAYOUT_GENERAL;


	return VK_IMAGE_LAYOUT_UNDEFINED;
}

VkAccessFlags VulkanResourceStateToAccessFlags(const ResourceState state)
{
	VkAccessFlags ret = 0;
	if (state & RESOURCE_STATE_COPY_SOURCE)
	{
		ret |= VK_ACCESS_TRANSFER_READ_BIT;
	}
	if (state & RESOURCE_STATE_COPY_DEST)
	{
		ret |= VK_ACCESS_TRANSFER_WRITE_BIT;
	}
	if (state & RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER)
	{
		ret |= VK_ACCESS_UNIFORM_READ_BIT | VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
	}
	if (state & RESOURCE_STATE_INDEX_BUFFER)
	{
		ret |= VK_ACCESS_INDEX_READ_BIT;
	}
	if (state & RESOURCE_STATE_UNORDERED_ACCESS)
	{
		ret |= VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
	}
	if (state & RESOURCE_STATE_INDIRECT_ARGUMENT)
	{
		ret |= VK_ACCESS_INDIRECT_COMMAND_READ_BIT;
	}
	if (state & RESOURCE_STATE_RENDER_TARGET)
	{
		ret |= VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	}
	if (state & RESOURCE_STATE_DEPTH_WRITE)
	{
		ret |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	}
	if (state & RESOURCE_STATE_DEPTH_READ)
	{
		ret |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
	}
	if (state & RESOURCE_STATE_ALL_SHADER_RESOURCE)
	{
		ret |= VK_ACCESS_SHADER_READ_BIT;
	}
	if (state & RESOURCE_STATE_PRESENT)
	{
		ret |= VK_ACCESS_MEMORY_READ_BIT;
	}

	return ret;
}

VkPipelineStageFlags VulkanResourceStateToPipelineStage(ResourceState state, QueueType queueType)
{
	VkPipelineStageFlags flags = 0;

	switch (queueType)
	{
	case QUEUE_TYPE_GRAPHICS:
	{
		if (state & RESOURCE_STATE_INDEX_BUFFER)
		{
			flags |= VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;
		}
		if (state & RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER)
		{
			flags |= VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;
			flags |= VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;
		}
		if (state & RESOURCE_STATE_PIXEL_SHADER_RESOURCE)
		{
			flags |= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		if (state & RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE)
		{
			flags |= VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;
			flags |= VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
		}
		if (state & RESOURCE_STATE_UNORDERED_ACCESS)
		{
			flags |= VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
			flags |= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		if (state & RESOURCE_STATE_RENDER_TARGET)
		{
			flags |= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		}
		if (state & (RESOURCE_STATE_DEPTH_READ | RESOURCE_STATE_DEPTH_WRITE))
		{
			flags |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
		}
		break;
	}
	case QUEUE_TYPE_COMPUTE:
	{
		flags |= VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
	}

	case QUEUE_TYPE_COPY:
	{
		return VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
	}
	default:
		break;
	}

	// Compatible with both compute and graphics queues
	if (state & RESOURCE_STATE_INDIRECT_ARGUMENT)
	{
		flags |= VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT;
	}
	if (state & (RESOURCE_STATE_COPY_DEST | RESOURCE_STATE_COPY_SOURCE))
	{
		flags |= VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	if (flags == 0)
	{
		flags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
	}

	return flags;
}

void VulkanResourceBarrier(const CommandBuffer* const pCommandBuffer, const uint32_t numBarrierInfos, const BarrierInfo* const pBarrierInfos)
{
	VkImageMemoryBarrier imageBarrier[MAX_NUM_BARRIERS]{};
	uint32_t imageBarrierCount = 0;

	VkBufferMemoryBarrier bufferBarrier[MAX_NUM_BARRIERS]{};
	uint32_t bufferBarrierCount = 0;

	VkPipelineStageFlags beforeStageFlags = 0;
	VkPipelineStageFlags afterStageFlags = 0;

	for (uint32_t i = 0; i < numBarrierInfos; ++i)
	{
		if (pBarrierInfos[i].type == BARRIER_TYPE_BUFFER)
		{
			if (pBarrierInfos[i].currentState == RESOURCE_STATE_UNORDERED_ACCESS && pBarrierInfos[i].newState == RESOURCE_STATE_UNORDERED_ACCESS)
			{
				bufferBarrier[bufferBarrierCount].srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
				bufferBarrier[bufferBarrierCount].dstAccessMask = VK_ACCESS_SHADER_WRITE_BIT | VK_ACCESS_SHADER_READ_BIT;
			}
			else
			{
				bufferBarrier[bufferBarrierCount].srcAccessMask = VulkanResourceStateToAccessFlags(pBarrierInfos[i].currentState);
				bufferBarrier[bufferBarrierCount].dstAccessMask = VulkanResourceStateToAccessFlags(pBarrierInfos[i].newState);
			}

			bufferBarrier[bufferBarrierCount].sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
			bufferBarrier[bufferBarrierCount].pNext = nullptr;
			bufferBarrier[bufferBarrierCount].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			bufferBarrier[bufferBarrierCount].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			bufferBarrier[bufferBarrierCount].buffer = pBarrierInfos[i].pBuffer->vk.buffer;
			bufferBarrier[bufferBarrierCount].offset = 0;
			bufferBarrier[bufferBarrierCount].size = pBarrierInfos[i].pBuffer->size;

			beforeStageFlags |= VulkanResourceStateToPipelineStage(pBarrierInfos[i].currentState, pCommandBuffer->type);
			afterStageFlags |= VulkanResourceStateToPipelineStage(pBarrierInfos[i].newState, pCommandBuffer->type);

			++bufferBarrierCount;
		}
		else if (pBarrierInfos[i].type == BARRIER_TYPE_TEXTURE)
		{
			if (pBarrierInfos[i].currentState == RESOURCE_STATE_UNORDERED_ACCESS && pBarrierInfos[i].newState == RESOURCE_STATE_UNORDERED_ACCESS)
			{
				imageBarrier[imageBarrierCount].srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
				imageBarrier[imageBarrierCount].dstAccessMask = VK_ACCESS_SHADER_WRITE_BIT | VK_ACCESS_SHADER_READ_BIT;
				imageBarrier[imageBarrierCount].oldLayout = VK_IMAGE_LAYOUT_GENERAL;
				imageBarrier[imageBarrierCount].newLayout = VK_IMAGE_LAYOUT_GENERAL;
			}
			else
			{
				imageBarrier[imageBarrierCount].srcAccessMask = VulkanResourceStateToAccessFlags(pBarrierInfos[i].currentState);
				imageBarrier[imageBarrierCount].dstAccessMask = VulkanResourceStateToAccessFlags(pBarrierInfos[i].newState);
				imageBarrier[imageBarrierCount].oldLayout = VulkanResourceStateToImageLayout(pBarrierInfos[i].currentState);
				imageBarrier[imageBarrierCount].newLayout = VulkanResourceStateToImageLayout(pBarrierInfos[i].newState);
			}

			imageBarrier[imageBarrierCount].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			imageBarrier[imageBarrierCount].pNext = nullptr;
			imageBarrier[imageBarrierCount].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			imageBarrier[imageBarrierCount].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			imageBarrier[imageBarrierCount].image = pBarrierInfos[i].pTexture->vk.image;
			imageBarrier[imageBarrierCount].subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			imageBarrier[imageBarrierCount].subresourceRange.baseMipLevel = 0;
			imageBarrier[imageBarrierCount].subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
			imageBarrier[imageBarrierCount].subresourceRange.baseArrayLayer = 0;
			imageBarrier[imageBarrierCount].subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;

			beforeStageFlags |= VulkanResourceStateToPipelineStage(pBarrierInfos[i].currentState, pCommandBuffer->type);
			afterStageFlags |= VulkanResourceStateToPipelineStage(pBarrierInfos[i].newState, pCommandBuffer->type);

			++imageBarrierCount;
		}
		else //BARRIER_TYPE_RENDER_TARGET
		{
			bool isDepth = TinyImageFormat_IsDepthOnly(pBarrierInfos[i].pRenderTarget->info.format)
				|| TinyImageFormat_IsDepthAndStencil(pBarrierInfos[i].pRenderTarget->info.format);

			if (pBarrierInfos[i].currentState == RESOURCE_STATE_UNORDERED_ACCESS && pBarrierInfos[i].newState == RESOURCE_STATE_UNORDERED_ACCESS)
			{
				imageBarrier[imageBarrierCount].srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
				imageBarrier[imageBarrierCount].dstAccessMask = VK_ACCESS_SHADER_WRITE_BIT | VK_ACCESS_SHADER_READ_BIT;
				imageBarrier[imageBarrierCount].oldLayout = VK_IMAGE_LAYOUT_GENERAL;
				imageBarrier[imageBarrierCount].newLayout = VK_IMAGE_LAYOUT_GENERAL;
			}
			else
			{
				imageBarrier[imageBarrierCount].srcAccessMask = VulkanResourceStateToAccessFlags(pBarrierInfos[i].currentState);
				imageBarrier[imageBarrierCount].dstAccessMask = VulkanResourceStateToAccessFlags(pBarrierInfos[i].newState);
				imageBarrier[imageBarrierCount].oldLayout = VulkanResourceStateToImageLayout(pBarrierInfos[i].currentState);
				imageBarrier[imageBarrierCount].newLayout = VulkanResourceStateToImageLayout(pBarrierInfos[i].newState);
			}

			imageBarrier[imageBarrierCount].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			imageBarrier[imageBarrierCount].pNext = nullptr;
			imageBarrier[imageBarrierCount].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			imageBarrier[imageBarrierCount].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			imageBarrier[imageBarrierCount].image = pBarrierInfos[i].pRenderTarget->vk.image;
			imageBarrier[imageBarrierCount].subresourceRange.aspectMask = (isDepth) ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
			imageBarrier[imageBarrierCount].subresourceRange.baseMipLevel = 0;
			imageBarrier[imageBarrierCount].subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
			imageBarrier[imageBarrierCount].subresourceRange.baseArrayLayer = 0;
			imageBarrier[imageBarrierCount].subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;

			beforeStageFlags |= VulkanResourceStateToPipelineStage(pBarrierInfos[i].currentState, pCommandBuffer->type);
			afterStageFlags |= VulkanResourceStateToPipelineStage(pBarrierInfos[i].newState, pCommandBuffer->type);

			++imageBarrierCount;
		}
	}

	vkCmdPipelineBarrier(pCommandBuffer->vk.commandBuffer, beforeStageFlags, afterStageFlags,
		0, 0, nullptr, bufferBarrierCount, bufferBarrier, imageBarrierCount, imageBarrier);
}

void VulkanInitialTransition(const Renderer* const pRenderer, const BarrierInfo* const pBarrierInfo)
{
	VulkanResetCommandBuffer(pRenderer, &pRenderer->commandBuffer);

	VulkanBeginCommandBuffer(&pRenderer->commandBuffer);

	VulkanResourceBarrier(&pRenderer->commandBuffer, 1, pBarrierInfo);

	VulkanEndCommandBuffer(&pRenderer->commandBuffer);

	QueueSubmitInfo submitInfo{};
	submitInfo.pCommandBuffer = &pRenderer->commandBuffer;
	submitInfo.pFence = nullptr;
	submitInfo.numSignalSemaphores = 0;
	submitInfo.signalSemaphores = nullptr;
	submitInfo.numWaitSemaphores = 0;
	submitInfo.waitSemaphores = nullptr;
	submitInfo.pQueue = &pRenderer->queue;
	VulkanQueueSubmit(&submitInfo);

	Queue queue = pRenderer->queue;
	VulkanWaitQueueIdle(pRenderer, &queue);
}

void VulkanCreateRenderTarget(const Renderer* const pRenderer, const RenderTargetInfo* const pInfo, RenderTarget* pRenderTarget)
{
	bool isDepth = TinyImageFormat_IsDepthOnly(pInfo->format) || TinyImageFormat_IsDepthAndStencil(pInfo->format);

	VkImageCreateInfo createImageInfo{};
	createImageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	createImageInfo.pNext = nullptr;
	createImageInfo.flags = 0;
	createImageInfo.imageType = VK_IMAGE_TYPE_2D;
	createImageInfo.format = (VkFormat)TinyImageFormat_ToVkFormat(pInfo->format);
	createImageInfo.extent.width = pInfo->width;
	createImageInfo.extent.height = pInfo->height;
	createImageInfo.extent.depth = 1;
	createImageInfo.mipLevels = 1;
	createImageInfo.arrayLayers = 1;
	createImageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	createImageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	createImageInfo.usage = (isDepth) ? VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT : VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	createImageInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
	createImageInfo.queueFamilyIndexCount = 3;
	createImageInfo.pQueueFamilyIndices = pRenderer->vk.familyIndices;

	createImageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

	VmaAllocationCreateInfo allocCreateInfo = {};
	allocCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
	allocCreateInfo.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
	allocCreateInfo.priority = 1.0f;

	VULKAN_ERROR_CHECK(vmaCreateImage(pRenderer->vk.allocator, &createImageInfo,
		&allocCreateInfo, &pRenderTarget->vk.image, &pRenderTarget->vk.allocation, nullptr));

	VkImageViewCreateInfo createImageViewInfo{};
	createImageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	createImageViewInfo.pNext = nullptr;
	createImageViewInfo.flags = 0;
	createImageViewInfo.image = pRenderTarget->vk.image;
	createImageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	createImageViewInfo.format = (VkFormat)TinyImageFormat_ToVkFormat(pInfo->format);
	createImageViewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	createImageViewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	createImageViewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	createImageViewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
	createImageViewInfo.subresourceRange.aspectMask = (isDepth) ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
	createImageViewInfo.subresourceRange.baseMipLevel = 0;
	createImageViewInfo.subresourceRange.levelCount = 1;
	createImageViewInfo.subresourceRange.baseArrayLayer = 0;
	createImageViewInfo.subresourceRange.layerCount = 1;

	VULKAN_ERROR_CHECK(vkCreateImageView(pRenderer->vk.logicalDevice, &createImageViewInfo, nullptr, &pRenderTarget->vk.imageView));

	pRenderTarget->info = *pInfo;

	BarrierInfo barrier{};
	barrier.type = BARRIER_TYPE_RENDER_TARGET;
	barrier.pRenderTarget = pRenderTarget;
	barrier.currentState = RESOURCE_STATE_UNDEFINED;
	barrier.newState = pInfo->initialState;
	VulkanInitialTransition(pRenderer, &barrier);
}

void VulkanDestroyRenderTarget(const Renderer* const pRenderer, RenderTarget* attachment)
{
	vkDestroyImageView(pRenderer->vk.logicalDevice, attachment->vk.imageView, nullptr);
	vmaDestroyImage(pRenderer->vk.allocator, attachment->vk.image, attachment->vk.allocation);
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

void CreateSurface(const Renderer* const pRenderer, const Window* const window, VkSurfaceKHR* pOutSurface)
{
	VkWin32SurfaceCreateInfoKHR surfaceInfo{};
	surfaceInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	surfaceInfo.pNext = nullptr;
	surfaceInfo.flags = 0;
	surfaceInfo.hinstance = GetModuleHandle(nullptr);
	surfaceInfo.hwnd = window->wndHandle;

	VULKAN_ERROR_CHECK(vkCreateWin32SurfaceKHR(pRenderer->vk.instance, &surfaceInfo, nullptr, pOutSurface));
}

void QuerySwapChainSupport(const Renderer* const pRenderer, const SwapChain* pSwapChain, VulkanInternalSwapChainQueryInfo* pQueryInfo)
{
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(pRenderer->vk.physicalDevice, pSwapChain->vk.surface, &pQueryInfo->capabilities);

	vkGetPhysicalDeviceSurfaceFormatsKHR(pRenderer->vk.physicalDevice, pSwapChain->vk.surface, &pQueryInfo->formatCount, nullptr);

	if (pQueryInfo->formatCount == 0)
	{
		MessageBox(nullptr, L"No surface formats are supported by the GPU. Exiting Program.", L"Surface formats error.", MB_OK);
		exit(2);
	}
	pQueryInfo->formats = (VkSurfaceFormatKHR*)calloc(pQueryInfo->formatCount, sizeof(VkSurfaceFormatKHR));
	vkGetPhysicalDeviceSurfaceFormatsKHR(pRenderer->vk.physicalDevice, pSwapChain->vk.surface, &pQueryInfo->formatCount, pQueryInfo->formats);

	vkGetPhysicalDeviceSurfacePresentModesKHR(pRenderer->vk.physicalDevice, pSwapChain->vk.surface, &pQueryInfo->presentModeCount, nullptr);
	if (pQueryInfo->presentModeCount == 0)
	{
		MessageBox(nullptr, L"No present modes are supported by the GPU. Exiting Program.", L"Present modes error.", MB_OK);
		free(pQueryInfo->formats);
		exit(2);
	}
	pQueryInfo->presentModes = (VkPresentModeKHR*)calloc(pQueryInfo->presentModeCount, sizeof(VkPresentModeKHR));
	vkGetPhysicalDeviceSurfacePresentModesKHR(pRenderer->vk.physicalDevice, pSwapChain->vk.surface,
		&pQueryInfo->presentModeCount, pQueryInfo->presentModes);
}

VkSurfaceFormatKHR CheckSwapSurfaceFormat(const VulkanInternalSwapChainQueryInfo* const queryInfo,
	const SwapChainInfo* const info)
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
		if (queryInfo->presentModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR)
			return queryInfo->presentModes[i];
	}

	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D CheckSwapExtent(const VulkanInternalSwapChainQueryInfo* queryInfo, const SwapChainInfo* const info)
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

void VulkanCreateSwapChain(const Renderer* const pRenderer, const SwapChainInfo* const pInfo, SwapChain* pSwapChain)
{
	CreateSurface(pRenderer, pInfo->window, &pSwapChain->vk.surface);

	VulkanInternalSwapChainQueryInfo queryInfo{};
	QuerySwapChainSupport(pRenderer, pSwapChain, &queryInfo);

	VkSurfaceFormatKHR surfaceFormat = CheckSwapSurfaceFormat(&queryInfo, pInfo);
	VkPresentModeKHR presentMode = ChooseSwapPresentMode(&queryInfo);
	VkExtent2D extent = CheckSwapExtent(&queryInfo, pInfo);

	uint32_t imageCount = queryInfo.capabilities.minImageCount + 1;

	if (queryInfo.capabilities.maxImageCount > 0 && imageCount > queryInfo.capabilities.maxImageCount)
	{
		imageCount = queryInfo.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR swapCreateInfo{};
	swapCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapCreateInfo.pNext = nullptr;
	swapCreateInfo.flags = 0;
	swapCreateInfo.surface = pSwapChain->vk.surface;
	swapCreateInfo.minImageCount = imageCount;
	swapCreateInfo.imageFormat = surfaceFormat.format;
	swapCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
	swapCreateInfo.imageExtent = extent;
	swapCreateInfo.imageArrayLayers = 1;
	swapCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
	swapCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	swapCreateInfo.queueFamilyIndexCount = 0;
	swapCreateInfo.pQueueFamilyIndices = 0;
	swapCreateInfo.preTransform = queryInfo.capabilities.currentTransform;
	swapCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	swapCreateInfo.presentMode = presentMode;
	swapCreateInfo.clipped = VK_TRUE;
	swapCreateInfo.oldSwapchain = VK_NULL_HANDLE;

	VULKAN_ERROR_CHECK(vkCreateSwapchainKHR(pRenderer->vk.logicalDevice, &swapCreateInfo, nullptr, &pSwapChain->vk.swapChain));

	vkGetSwapchainImagesKHR(pRenderer->vk.logicalDevice, pSwapChain->vk.swapChain, &pSwapChain->numRenderTargets, nullptr);
	pSwapChain->pRenderTargets = (RenderTarget*)calloc(pSwapChain->numRenderTargets, sizeof(RenderTarget));
	VkImage* images = (VkImage*)calloc(pSwapChain->numRenderTargets, sizeof(VkImage));
	vkGetSwapchainImagesKHR(pRenderer->vk.logicalDevice, pSwapChain->vk.swapChain, &pSwapChain->numRenderTargets, images);

	for (uint32_t i = 0; i < pSwapChain->numRenderTargets; ++i)
	{
		pSwapChain->pRenderTargets[i].vk.image = images[i];
		pSwapChain->pRenderTargets[i].info.width = extent.width;
		pSwapChain->pRenderTargets[i].info.height = extent.height;
		pSwapChain->pRenderTargets[i].info.format = pInfo->format;
		pSwapChain->pRenderTargets[i].info.clearValue = pInfo->clearValue;
	}
	free(images);

	VulkanFreeSwapChainInfo(&queryInfo);

	for (uint32_t i = 0; i < pSwapChain->numRenderTargets; ++i)
	{
		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.pNext = nullptr;
		createInfo.flags = 0;
		createInfo.image = pSwapChain->pRenderTargets[i].vk.image;
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = (VkFormat)TinyImageFormat_ToVkFormat(pInfo->format);
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		VULKAN_ERROR_CHECK(vkCreateImageView(pRenderer->vk.logicalDevice, &createInfo, nullptr, &pSwapChain->pRenderTargets[i].vk.imageView));
	}

	for (uint32_t i = 0; i < pSwapChain->numRenderTargets; ++i)
	{
		BarrierInfo barrier{};
		barrier.type = BARRIER_TYPE_RENDER_TARGET;
		barrier.pRenderTarget = &pSwapChain->pRenderTargets[i];
		barrier.currentState = RESOURCE_STATE_UNDEFINED;
		barrier.newState = RESOURCE_STATE_PRESENT;
		VulkanInitialTransition(pRenderer, &barrier);
	}

	pSwapChain->info = *pInfo;
}

void VulkanDestroySwapChain(const Renderer* const pRenderer, SwapChain* pSwapChain)
{
	for (uint32_t i = 0; i < pSwapChain->numRenderTargets; ++i)
	{
		vkDestroyImageView(pRenderer->vk.logicalDevice, pSwapChain->pRenderTargets[i].vk.imageView, nullptr);
	}
	free(pSwapChain->pRenderTargets);
	vkDestroySwapchainKHR(pRenderer->vk.logicalDevice, pSwapChain->vk.swapChain, nullptr);
	vkDestroySurfaceKHR(pRenderer->vk.instance, pSwapChain->vk.surface, nullptr);
}
//-------------------------------------------------------------------------------------------------------------------------------------------

void VulkanInitUI(const Renderer* const pRenderer, const UIDesc* const pInfo)
{
	ImGui_ImplVulkan_InitInfo initInfo{};
	initInfo.Instance = pRenderer->vk.instance;
	initInfo.PhysicalDevice = pRenderer->vk.physicalDevice;
	initInfo.Device = pRenderer->vk.logicalDevice;
	initInfo.QueueFamily = pRenderer->vk.familyIndices[GRAPHICS_FAMILY];
	initInfo.Queue = pInfo->pQueue->vk.queue;
	initInfo.PipelineCache = nullptr;
	initInfo.DescriptorPool = nullptr;
	initInfo.DescriptorPoolSize = IMGUI_IMPL_VULKAN_MINIMUM_IMAGE_SAMPLER_POOL_SIZE + 1;
	initInfo.RenderPass = nullptr;
	initInfo.Subpass = 0;
	initInfo.MinImageCount = pInfo->numImages;
	initInfo.ImageCount = pInfo->numImages;
	initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
	initInfo.UseDynamicRendering = true;

	VkPipelineRenderingCreateInfo pipelineRendingerCreateInfo{};
	pipelineRendingerCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR;
	pipelineRendingerCreateInfo.pNext = nullptr;
	pipelineRendingerCreateInfo.viewMask = 0;
	pipelineRendingerCreateInfo.colorAttachmentCount = 1;
	VkFormat colorFormat = (VkFormat)TinyImageFormat_ToVkFormat(pInfo->renderTargetFormat);
	pipelineRendingerCreateInfo.pColorAttachmentFormats = &colorFormat;

	if (pInfo->depthFormat != TinyImageFormat_UNDEFINED)
	{
		pipelineRendingerCreateInfo.depthAttachmentFormat = (VkFormat)TinyImageFormat_ToVkFormat(pInfo->depthFormat);
	}

	initInfo.PipelineRenderingCreateInfo = pipelineRendingerCreateInfo;

	ImGui_ImplVulkan_Init(&initInfo);
}

void VulkanDestroyUI(const Renderer* const pRenderer)
{
	ImGui_ImplVulkan_Shutdown();
}

void VulkanRenderUI(const CommandBuffer* const pCommandBuffer)
{
	ImGui::Render();
	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), pCommandBuffer->vk.commandBuffer, nullptr);
}

//INIT AND DESTROY
//-------------------------------------------------------------------------------------------------------------------------------------------
void CreateInstance(Renderer* pRenderer, const char* appName)
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

	if (pRenderer->vk.enableValidationLayers)
	{
		const char* layers[] = { "VK_LAYER_KHRONOS_validation"};
		if (!CheckValidationLayerSupport(layers, 1))
		{
			MessageBox(nullptr, L"Validation Layers Requested, But Not Available! Exiting Program.", L"Validation Error", MB_OK);
			free(extNames);
			exit(2);
		}

		createInfo.enabledLayerCount = 1;
		createInfo.ppEnabledLayerNames = layers;

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

	VULKAN_ERROR_CHECK(vkCreateInstance(&createInfo, nullptr, &pRenderer->vk.instance));

	volkLoadInstance(pRenderer->vk.instance);

	free(extNames);
	free(extensions);
}

float gMaxSamplerAnisotropy = 0.0f;
void CreatePhysicalDevice(Renderer* pRenderer)
{
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(pRenderer->vk.instance, &deviceCount, nullptr);
	if (deviceCount == 0)
	{
		MessageBox(nullptr, L"Failed to find any GPUs with Vulkan Support. Exiting Program.", L"No Vulkan Supported GPU Found", MB_OK);
		exit(2);
	}

	VkPhysicalDevice* device = (VkPhysicalDevice*)calloc(deviceCount, sizeof(VkPhysicalDevice));
	vkEnumeratePhysicalDevices(pRenderer->vk.instance, &deviceCount, device);

	for (uint32_t i = 0; i < deviceCount; ++i)
	{
		VkPhysicalDeviceProperties deviceProperties;
		VkPhysicalDeviceFeatures deviceFeatures;
		vkGetPhysicalDeviceProperties(device[i], &deviceProperties);
		vkGetPhysicalDeviceFeatures(device[i], &deviceFeatures);

		if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
		{
			pRenderer->vk.physicalDevice = device[i];
			gMaxSamplerAnisotropy = deviceProperties.limits.maxSamplerAnisotropy;
			return;
		}
	}

	MessageBox(nullptr, L"Couldn't find a discrete gpu. Exiting Program.", L"No discrete GPU found.", MB_OK);
	exit(2);
}


void FindQueueFamilies(Renderer* pRenderer)
{
	bool graphicsFam = false;
	bool computeFam = false;
	bool transferFam = false;

	uint32_t queueFamilyPropertiesCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(pRenderer->vk.physicalDevice, &queueFamilyPropertiesCount, nullptr);
	
	VkQueueFamilyProperties* queueFamilyProperties = (VkQueueFamilyProperties*)calloc(queueFamilyPropertiesCount, sizeof(VkQueueFamilyProperties));
	vkGetPhysicalDeviceQueueFamilyProperties(pRenderer->vk.physicalDevice, &queueFamilyPropertiesCount, queueFamilyProperties);

	for (uint32_t i = 0; i < queueFamilyPropertiesCount; ++i)
	{
		char queueFamilyMsg[1024]{};
		snprintf(queueFamilyMsg, 1024, "Index i = %d\nQueue Family Flags = %d\nQueue Count = %d\n\n",
			i, queueFamilyProperties[i].queueFlags, queueFamilyProperties[i].queueCount);
		OutputDebugStringA(queueFamilyMsg);

		//Looking for a queue family that supports graphics
		if ((queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT))
		{
			snprintf(queueFamilyMsg, 1024, "Graphics queue family index = %d\nQueue Count = %d\n\n",
				i, queueFamilyProperties->queueCount);
			OutputDebugStringA(queueFamilyMsg);
			graphicsFam = true;
			pRenderer->vk.familyIndices[GRAPHICS_FAMILY] = i;
		}

		//Looking for a queue family that supports compute only (a dedicated compute queue)
		else if ((queueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT) &&
			(queueFamilyProperties[i].queueFlags & ~VK_QUEUE_GRAPHICS_BIT))
		{
			snprintf(queueFamilyMsg, 1024, "Compute queue family index = %d\nQueue Count = %d\n\n",
				i, queueFamilyProperties[i].queueCount);
			OutputDebugStringA(queueFamilyMsg);
			pRenderer->vk.familyIndices[COMPUTE_FAMILY] = i;
			computeFam = true;
		}

		//Looking for a queue family that supports transfer only
		else if ((queueFamilyProperties[i].queueFlags & VK_QUEUE_TRANSFER_BIT) &&
			(queueFamilyProperties[i].queueFlags & ~VK_QUEUE_GRAPHICS_BIT) &&
			(queueFamilyProperties[i].queueFlags & ~VK_QUEUE_COMPUTE_BIT))
		{
			snprintf(queueFamilyMsg, 1024, "Transfer queue family index = %d\nQueue Count = %d\n\n",
				i, queueFamilyProperties[i].queueCount);
			OutputDebugStringA(queueFamilyMsg);
			pRenderer->vk.familyIndices[TRANSFER_FAMILY] = i;
			transferFam = true;
		}
	}

	if (!(graphicsFam && computeFam && transferFam))
	{
		MessageBox(nullptr, L"Not all queue families are supported. Exiting Program.", L"All necessary queue families not found.", MB_OK);
		free(queueFamilyProperties);
		exit(2);
	}

	free(queueFamilyProperties);
}

void CreateLogicalDevice(Renderer* pRenderer)
{
	VkDeviceQueueCreateInfo* queueCreateInfos = (VkDeviceQueueCreateInfo*)calloc(3, sizeof(VkDeviceQueueCreateInfo));

	float queuePrio = 1.0f;

	//Graphics queue
	queueCreateInfos[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfos[0].pNext = nullptr;
	queueCreateInfos[0].flags = 0;
	queueCreateInfos[0].queueFamilyIndex = pRenderer->vk.familyIndices[GRAPHICS_FAMILY];
	queueCreateInfos[0].queueCount = 1;
	queueCreateInfos[0].pQueuePriorities = &queuePrio;

	//Dedicated compute queue
	queueCreateInfos[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfos[1].pNext = nullptr;
	queueCreateInfos[1].flags = 0;
	queueCreateInfos[1].queueFamilyIndex = pRenderer->vk.familyIndices[COMPUTE_FAMILY];
	queueCreateInfos[1].queueCount = 1;
	queueCreateInfos[1].pQueuePriorities = &queuePrio;

	//Dedicated transfer queue
	queueCreateInfos[2].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfos[2].pNext = nullptr;
	queueCreateInfos[2].flags = 0;
	queueCreateInfos[2].queueFamilyIndex = pRenderer->vk.familyIndices[TRANSFER_FAMILY];
	queueCreateInfos[2].queueCount = 1;
	queueCreateInfos[2].pQueuePriorities = &queuePrio;

	VkPhysicalDeviceFeatures deviceFeatures{};
	deviceFeatures.samplerAnisotropy = true;

	const char* extensions[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME , VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME};
	if (!CheckDeviceExtensionSupport(pRenderer, extensions, 2))
	{
		MessageBox(nullptr, L"One or more device extensions are not avaiable. Exiting Program.", L"Device extensions error.", MB_OK);
		free(queueCreateInfos);
		exit(2);
	}

	VkPhysicalDeviceDynamicRenderingFeaturesKHR dynamicRenderingFeatures{};
	dynamicRenderingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR;
	dynamicRenderingFeatures.pNext = nullptr;
	dynamicRenderingFeatures.dynamicRendering = true;

	VkDeviceCreateInfo deviceCreateInfo{};
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.pNext = &dynamicRenderingFeatures;
	deviceCreateInfo.queueCreateInfoCount = 3;
	deviceCreateInfo.pQueueCreateInfos = queueCreateInfos;
	deviceCreateInfo.enabledExtensionCount = 2;
	deviceCreateInfo.ppEnabledExtensionNames = extensions;
	deviceCreateInfo.pEnabledFeatures = &deviceFeatures;

	VULKAN_ERROR_CHECK(vkCreateDevice(pRenderer->vk.physicalDevice, &deviceCreateInfo, nullptr, &pRenderer->vk.logicalDevice));

	volkLoadDevice(pRenderer->vk.logicalDevice);

	free(queueCreateInfos);
}

void VmaInit(Renderer* pRenderer)
{
	VmaVulkanFunctions vulkanFunctions = {};
	vulkanFunctions.vkGetInstanceProcAddr = vkGetInstanceProcAddr;
	vulkanFunctions.vkGetDeviceProcAddr = vkGetDeviceProcAddr;
	vulkanFunctions.vkAllocateMemory = vkAllocateMemory;
	vulkanFunctions.vkBindBufferMemory = vkBindBufferMemory;
	vulkanFunctions.vkBindImageMemory = vkBindImageMemory;
	vulkanFunctions.vkCreateBuffer = vkCreateBuffer;
	vulkanFunctions.vkCreateImage = vkCreateImage;
	vulkanFunctions.vkDestroyBuffer = vkDestroyBuffer;
	vulkanFunctions.vkDestroyImage = vkDestroyImage;
	vulkanFunctions.vkFreeMemory = vkFreeMemory;
	vulkanFunctions.vkGetBufferMemoryRequirements = vkGetBufferMemoryRequirements;
	vulkanFunctions.vkGetBufferMemoryRequirements2KHR = vkGetBufferMemoryRequirements2KHR;
	vulkanFunctions.vkGetImageMemoryRequirements = vkGetImageMemoryRequirements;
	vulkanFunctions.vkGetImageMemoryRequirements2KHR = vkGetImageMemoryRequirements2KHR;
	vulkanFunctions.vkGetPhysicalDeviceMemoryProperties = vkGetPhysicalDeviceMemoryProperties;
	vulkanFunctions.vkGetPhysicalDeviceProperties = vkGetPhysicalDeviceProperties;
	vulkanFunctions.vkMapMemory = vkMapMemory;
	vulkanFunctions.vkUnmapMemory = vkUnmapMemory;
	vulkanFunctions.vkFlushMappedMemoryRanges = vkFlushMappedMemoryRanges;
	vulkanFunctions.vkInvalidateMappedMemoryRanges = vkInvalidateMappedMemoryRanges;
	vulkanFunctions.vkCmdCopyBuffer = vkCmdCopyBuffer;
#if VMA_BIND_MEMORY2 || VMA_VULKAN_VERSION >= 1001000
	/// Fetch "vkBindBufferMemory2" on Vulkan >= 1.1, fetch "vkBindBufferMemory2KHR" when using VK_KHR_bind_memory2 extension.
	vulkanFunctions.vkBindBufferMemory2KHR = vkBindBufferMemory2KHR;
	/// Fetch "vkBindImageMemory2" on Vulkan >= 1.1, fetch "vkBindImageMemory2KHR" when using VK_KHR_bind_memory2 extension.
	vulkanFunctions.vkBindImageMemory2KHR = vkBindImageMemory2KHR;
#endif
#if VMA_MEMORY_BUDGET || VMA_VULKAN_VERSION >= 1001000
	vulkanFunctions.vkGetPhysicalDeviceMemoryProperties2KHR =
		vkGetPhysicalDeviceMemoryProperties2KHR ? vkGetPhysicalDeviceMemoryProperties2KHR : vkGetPhysicalDeviceMemoryProperties2;
#endif
#if VMA_VULKAN_VERSION >= 1003000
	/// Fetch from "vkGetDeviceBufferMemoryRequirements" on Vulkan >= 1.3, but you can also fetch it from
	/// "vkGetDeviceBufferMemoryRequirementsKHR" if you enabled extension VK_KHR_maintenance4.
	vulkanFunctions.vkGetDeviceBufferMemoryRequirements = vkGetDeviceBufferMemoryRequirements;
	/// Fetch from "vkGetDeviceImageMemoryRequirements" on Vulkan >= 1.3, but you can also fetch it from
	/// "vkGetDeviceImageMemoryRequirementsKHR" if you enabled extension VK_KHR_maintenance4.
	vulkanFunctions.vkGetDeviceImageMemoryRequirements = vkGetDeviceImageMemoryRequirements;
#endif

	VmaAllocatorCreateInfo allocatorCreateInfo = {};
	allocatorCreateInfo.flags = 0;
	allocatorCreateInfo.vulkanApiVersion = VK_API_VERSION_1_3;
	allocatorCreateInfo.physicalDevice = pRenderer->vk.physicalDevice;
	allocatorCreateInfo.device = pRenderer->vk.logicalDevice;
	allocatorCreateInfo.instance = pRenderer->vk.instance;
	allocatorCreateInfo.pVulkanFunctions = &vulkanFunctions;

	vmaCreateAllocator(&allocatorCreateInfo, &pRenderer->vk.allocator);
}

void VmaDestroy(Renderer* pRenderer)
{
	vmaDestroyAllocator(pRenderer->vk.allocator);
}

void VulkanCreateQueue(const Renderer* const pRenderer, const QueueType queueType, Queue* pQueue)
{
	switch (queueType)
	{
	case QUEUE_TYPE_GRAPHICS:
		vkGetDeviceQueue(pRenderer->vk.logicalDevice, pRenderer->vk.familyIndices[GRAPHICS_FAMILY], 0, &pQueue->vk.queue);
		return;

	case QUEUE_TYPE_COMPUTE:
		vkGetDeviceQueue(pRenderer->vk.logicalDevice, pRenderer->vk.familyIndices[COMPUTE_FAMILY], 0, &pQueue->vk.queue);
		return;

	case QUEUE_TYPE_COPY:
		vkGetDeviceQueue(pRenderer->vk.logicalDevice, pRenderer->vk.familyIndices[TRANSFER_FAMILY], 0, &pQueue->vk.queue);
		return;
	}
}

void VulkanDestroyQueue(const Renderer* const pRenderer, Queue* pQueue)
{
}

struct VulkanCopyEngine
{
	Queue queue;
	CommandBuffer commandBuffer;
	Buffer buffer;
}gVulkanCopyEngine;

void VulkanInitCopyEngine(const Renderer* const pRenderer)
{
	VulkanCreateQueue(pRenderer, QUEUE_TYPE_COPY, &gVulkanCopyEngine.queue);

	VulkanCreateCommandBuffer(pRenderer, QUEUE_TYPE_COPY, &gVulkanCopyEngine.commandBuffer);
}

void VulkanBeginCopyEngineCommandBuffer(const Renderer* const pRenderer)
{
	VulkanResetCommandBuffer(pRenderer, &gVulkanCopyEngine.commandBuffer);
	VulkanBeginCommandBuffer(&gVulkanCopyEngine.commandBuffer);
}

void VulkanEndCopyEngineCommandBuffer(const Renderer* const pRenderer)
{
	VulkanEndCommandBuffer(&gVulkanCopyEngine.commandBuffer);

	QueueSubmitInfo submitInfo{};
	submitInfo.pQueue = &gVulkanCopyEngine.queue;
	submitInfo.numWaitSemaphores = 0;
	submitInfo.waitSemaphores = nullptr;
	submitInfo.numSignalSemaphores = 0;
	submitInfo.signalSemaphores = nullptr;
	submitInfo.pFence = nullptr;
	submitInfo.pCommandBuffer = &gVulkanCopyEngine.commandBuffer;
	VulkanQueueSubmit(&submitInfo);

	VulkanWaitQueueIdle(pRenderer, &gVulkanCopyEngine.queue);
}

void VulkanDestroyCopyEngine(const Renderer* const pRenderer)
{
	VulkanDestroyCommandBuffer(pRenderer, &gVulkanCopyEngine.commandBuffer);
}

#define MAX_NUM_DESCRIPTORS 1000
VkDescriptorPool gDescriptorPool;
void VulkanCreateDescriptorPool(const Renderer* const pRenderer)
{
	VkDescriptorPoolSize poolSize[6]{};

	poolSize[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSize[0].descriptorCount = MAX_NUM_DESCRIPTORS;

	poolSize[1].type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
	poolSize[1].descriptorCount = MAX_NUM_DESCRIPTORS;

	poolSize[2].type = VK_DESCRIPTOR_TYPE_SAMPLER;
	poolSize[2].descriptorCount = MAX_NUM_DESCRIPTORS;

	poolSize[3].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	poolSize[3].descriptorCount = MAX_NUM_DESCRIPTORS;

	poolSize[4].type = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
	poolSize[4].descriptorCount = MAX_NUM_DESCRIPTORS;

	poolSize[5].type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	poolSize[5].descriptorCount = MAX_NUM_DESCRIPTORS;

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.pNext = nullptr;
	poolInfo.flags = 0;
	poolInfo.maxSets = MAX_NUM_DESCRIPTORS;
	poolInfo.poolSizeCount = 6;
	poolInfo.pPoolSizes = poolSize;

	VULKAN_ERROR_CHECK(vkCreateDescriptorPool(pRenderer->vk.logicalDevice, &poolInfo, nullptr, &gDescriptorPool));
}

void VulkanDestroyDescriptorPool(const Renderer* const pRenderer)
{
	vkDestroyDescriptorPool(pRenderer->vk.logicalDevice, gDescriptorPool, nullptr);
}

void VulkanInitRenderer(Renderer* pRenderer, const char* appName)
{		
	VULKAN_ERROR_CHECK(volkInitialize());

	CreateInstance(pRenderer, appName);

#ifdef _DEBUG
	if (pRenderer->vk.enableValidationLayers)
	{
		SetupDebugMessenger(pRenderer);
	}
#endif

	CreatePhysicalDevice(pRenderer);

	FindQueueFamilies(pRenderer);

	CreateLogicalDevice(pRenderer);

	VmaInit(pRenderer);

	VulkanInitCopyEngine(pRenderer);

	VulkanCreateDescriptorPool(pRenderer);

	VulkanCreateQueue(pRenderer, QUEUE_TYPE_GRAPHICS, &pRenderer->queue);

	VulkanCreateCommandBuffer(pRenderer, QUEUE_TYPE_GRAPHICS, &pRenderer->commandBuffer);
}

void VulkanDestroyRenderer(Renderer* pRenderer)
{
	VulkanDestroyCommandBuffer(pRenderer, &pRenderer->commandBuffer);

	VulkanDestroyQueue(pRenderer, &pRenderer->queue);

	VulkanDestroyDescriptorPool(pRenderer);

	VulkanDestroyCopyEngine(pRenderer);

	VmaDestroy(pRenderer);

	vkDestroyDevice(pRenderer->vk.logicalDevice, nullptr);

	if (pRenderer->vk.enableValidationLayers)
	{
		DestroyDebugUtilsMessengerEXT(pRenderer->vk.instance, pRenderer->vk.debugMessenger, nullptr);
	}

	vkDestroyInstance(pRenderer->vk.instance, nullptr);

	volkFinalize();
}
//-------------------------------------------------------------------------------------------------------------------------------------------

//SHADERS
//-------------------------------------------------------------------------------------------------------------------------------------------
void VulkanCreateShader(const Renderer* const pRenderer, const ShaderInfo* const pInfo, Shader* pShader)
{
	char currentDirectory[MAX_FILE_PATH]{};
	GetCurrentPath(currentDirectory);

	char inputFileWithPath[MAX_FILE_PATH]{};
	strcat_s(inputFileWithPath, currentDirectory);
	strcat_s(inputFileWithPath, "CompiledShaders\\GLSL\\");
	strcat_s(inputFileWithPath, pInfo->filename);

	char* buffer = nullptr;
	uint32_t fileSize = 0;
	ReadFile(inputFileWithPath, &buffer, &fileSize, BINARY);

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

	VULKAN_ERROR_CHECK(vkCreateShaderModule(pRenderer->vk.logicalDevice, &createInfo, nullptr, &pShader->vk.shaderModule));

	VkPipelineShaderStageCreateInfo stageCreateInfo{};
	stageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	stageCreateInfo.pNext = nullptr;
	stageCreateInfo.flags = 0;
	switch (pInfo->type)
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
	stageCreateInfo.module = pShader->vk.shaderModule;
	stageCreateInfo.pName = "main";
	stageCreateInfo.pSpecializationInfo = nullptr;

	pShader->vk.shaderCreateInfo = stageCreateInfo;

	free(buffer);
	free(code);
}

void VulkanDestroyShader(const Renderer* const pRenderer, Shader* pShader)
{
	vkDestroyShaderModule(pRenderer->vk.logicalDevice, pShader->vk.shaderModule, nullptr);
}
//-------------------------------------------------------------------------------------------------------------------------------------------

//PIPELINE
//-------------------------------------------------------------------------------------------------------------------------------------------
void VulkanCreateRootSignature(const Renderer* const pRenderer, const RootSignatureInfo* const pInfo, RootSignature* pRootSignature)
{

	VkDescriptorSetLayoutBinding* perNoneBindings = nullptr;
	VkDescriptorSetLayoutBinding* perDrawBindings = nullptr;
	VkDescriptorSetLayoutBinding* perFrameBindings = nullptr;
	uint32_t stage{};

	for (uint32_t i = 0; i < pInfo->numRootParameterInfos; ++i)
	{
		VkDescriptorType type{};

		switch (pInfo->pRootParameterInfos[i].type)
		{
		case DESCRIPTOR_TYPE_TEXTURE:
			type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
			break;

		case DESCRIPTOR_TYPE_RW_TEXTURE:
			type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
			break;

		case DESCRIPTOR_TYPE_BUFFER:
			type = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
			break;

		case DESCRIPTOR_TYPE_RW_BUFFER:
			type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			break;

		case DESCRIPTOR_TYPE_UNIFORM_BUFFER:
			type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			break;

		case DESCRIPTOR_TYPE_SAMPLER:
			type = VK_DESCRIPTOR_TYPE_SAMPLER;
			break;
		}

		if (pInfo->pRootParameterInfos[i].stages & STAGE_VERTEX)
		{
			stage |= VK_SHADER_STAGE_VERTEX_BIT;
		}

		if (pInfo->pRootParameterInfos[i].stages & STAGE_PIXEL)
		{
			stage |= VK_SHADER_STAGE_FRAGMENT_BIT;
		}

		if (pInfo->pRootParameterInfos[i].stages & STAGE_COMPUTE)
		{
			stage |= VK_SHADER_STAGE_COMPUTE_BIT;
		}

		if (pInfo->pRootParameterInfos[i].updateFrequency == UPDATE_FREQUENCY_PER_NONE)
		{
			VkDescriptorSetLayoutBinding perNone{};

			perNone.descriptorType = type;
			perNone.stageFlags = stage;
			perNone.descriptorCount = pInfo->pRootParameterInfos[i].numDescriptors;
			perNone.binding = pInfo->pRootParameterInfos[i].binding;
			perNone.pImmutableSamplers = nullptr;

			arrpush(perNoneBindings, perNone);
		}
		else if (pInfo->pRootParameterInfos[i].updateFrequency == UPDATE_FREQUENCY_PER_FRAME)
		{
			VkDescriptorSetLayoutBinding perFrame{};

			perFrame.descriptorType = type;
			perFrame.stageFlags = stage;
			perFrame.descriptorCount = pInfo->pRootParameterInfos[i].numDescriptors;
			perFrame.binding = pInfo->pRootParameterInfos[i].binding;
			perFrame.pImmutableSamplers = nullptr;

			arrpush(perFrameBindings, perFrame);
		}
		else //UPDATE_FREQUENCY_PER_NONE
		{
			VkDescriptorSetLayoutBinding perDraw{};

			perDraw.descriptorType = type;
			perDraw.stageFlags = stage;
			perDraw.descriptorCount = pInfo->pRootParameterInfos[i].numDescriptors;
			perDraw.binding = pInfo->pRootParameterInfos[i].binding;
			perDraw.pImmutableSamplers = nullptr;

			arrpush(perDrawBindings, perDraw);
		}
	}

	VkDescriptorSetLayoutCreateInfo layoutInfoPerNone{};
	layoutInfoPerNone.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfoPerNone.pNext = nullptr;
	layoutInfoPerNone.flags = 0;
	layoutInfoPerNone.bindingCount = arrlenu(perNoneBindings);
	layoutInfoPerNone.pBindings = perNoneBindings;

	VULKAN_ERROR_CHECK(vkCreateDescriptorSetLayout(pRenderer->vk.logicalDevice, &layoutInfoPerNone, 
		nullptr, &pRootSignature->vk.descriptorSetLayouts[UPDATE_FREQUENCY_PER_NONE]))

	VkDescriptorSetLayoutCreateInfo layoutInfoPerFrame{};
	layoutInfoPerFrame.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfoPerFrame.pNext = nullptr;
	layoutInfoPerFrame.flags = 0;
	layoutInfoPerFrame.bindingCount = arrlenu(perFrameBindings);
	layoutInfoPerFrame.pBindings = perFrameBindings;

	VULKAN_ERROR_CHECK(vkCreateDescriptorSetLayout(pRenderer->vk.logicalDevice, &layoutInfoPerFrame,
		nullptr, &pRootSignature->vk.descriptorSetLayouts[UPDATE_FREQUENCY_PER_FRAME]))


	VkDescriptorSetLayoutCreateInfo layoutInfoPerDraw{};
	layoutInfoPerDraw.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfoPerDraw.pNext = nullptr;
	layoutInfoPerDraw.flags = 0;
	layoutInfoPerDraw.bindingCount = arrlenu(perDrawBindings);
	layoutInfoPerDraw.pBindings = perDrawBindings;

	VULKAN_ERROR_CHECK(vkCreateDescriptorSetLayout(pRenderer->vk.logicalDevice, &layoutInfoPerDraw,
		nullptr, &pRootSignature->vk.descriptorSetLayouts[UPDATE_FREQUENCY_PER_DRAW]))

	

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.pNext = nullptr;
	pipelineLayoutInfo.flags = 0;
	pipelineLayoutInfo.setLayoutCount = UPDATE_FREQUENCY_COUNT;
	pipelineLayoutInfo.pSetLayouts = pRootSignature->vk.descriptorSetLayouts;
	pipelineLayoutInfo.pushConstantRangeCount = 0;
	pipelineLayoutInfo.pPushConstantRanges = nullptr;

	if (pInfo->useRootConstants == true)
	{
		VkPushConstantRange pushConstant{};
		pushConstant.offset = 0;
		pushConstant.size = pInfo->rootConstantsInfo.numValues * pInfo->rootConstantsInfo.stride;

		if (pInfo->rootConstantsInfo.stages & STAGE_VERTEX)
		{
			stage |= VK_SHADER_STAGE_VERTEX_BIT;
		}

		if (pInfo->rootConstantsInfo.stages & STAGE_PIXEL)
		{
			stage |= VK_SHADER_STAGE_FRAGMENT_BIT;
		}

		if (pInfo->rootConstantsInfo.stages & STAGE_COMPUTE)
		{
			stage |= VK_SHADER_STAGE_COMPUTE_BIT;
		}
		pushConstant.stageFlags = stage;
		pRootSignature->vk.rootConstantStages = stage;

		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstant;
	}

	VULKAN_ERROR_CHECK(vkCreatePipelineLayout(pRenderer->vk.logicalDevice, &pipelineLayoutInfo, nullptr, &pRootSignature->vk.pipelineLayout));

	arrfree(perNoneBindings);
	arrfree(perFrameBindings);
	arrfree(perDrawBindings);
}

void VulkanDestroyRootSignature(const Renderer* const pRenderer, RootSignature* pRootSignature)
{
	vkDestroyPipelineLayout(pRenderer->vk.logicalDevice, pRootSignature->vk.pipelineLayout, nullptr);
	vkDestroyDescriptorSetLayout(pRenderer->vk.logicalDevice, pRootSignature->vk.descriptorSetLayouts[UPDATE_FREQUENCY_PER_DRAW], nullptr);
	vkDestroyDescriptorSetLayout(pRenderer->vk.logicalDevice, pRootSignature->vk.descriptorSetLayouts[UPDATE_FREQUENCY_PER_FRAME], nullptr);
	vkDestroyDescriptorSetLayout(pRenderer->vk.logicalDevice, pRootSignature->vk.descriptorSetLayouts[UPDATE_FREQUENCY_PER_NONE], nullptr);
}

void VulkanCreateInputAssemblyInfo(const PipelineInfo* const pInfo, VkPipelineInputAssemblyStateCreateInfo* pInputAssemblyInfo)
{
	pInputAssemblyInfo->sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	pInputAssemblyInfo->pNext = nullptr;
	pInputAssemblyInfo->flags = 0;

	switch (pInfo->topology)
	{
	case TOPOLOGY_POINT_LIST:
		pInputAssemblyInfo->topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
		break;

	case TOPOLOGY_LINE_LIST:
		pInputAssemblyInfo->topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
		break;

	case TOPOLOGY_TRIANGLE_LIST:
		pInputAssemblyInfo->topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		break;
	}

	pInputAssemblyInfo->primitiveRestartEnable = false;
}

void VulkanCreateRasterizerInfo(const PipelineInfo* const pInfo, VkPipelineRasterizationStateCreateInfo* pRasInfo)
{
	pRasInfo->sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	pRasInfo->pNext = nullptr;
	pRasInfo->flags = 0;
	pRasInfo->depthClampEnable = false;
	pRasInfo->rasterizerDiscardEnable = false;

	switch (pInfo->rasInfo.cullMode)
	{
	case CULL_MODE_NONE:
		pRasInfo->cullMode = VK_CULL_MODE_NONE;
		break;

	case CULL_MODE_FRONT:
		pRasInfo->cullMode = VK_CULL_MODE_FRONT_BIT;
		break;

	case CULL_MODE_BACK:
		pRasInfo->cullMode = VK_CULL_MODE_BACK_BIT;
		break;
	}

	switch (pInfo->rasInfo.faceMode)
	{
	case FACE_CLOCKWISE:
		pRasInfo->frontFace = VK_FRONT_FACE_CLOCKWISE;
		break;

	case FACE_COUNTER_CLOCKWISE:
		pRasInfo->frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		break;
	}

	switch (pInfo->rasInfo.fillMode)
	{
	case FILL_MODE_SOLID:
		pRasInfo->polygonMode = VK_POLYGON_MODE_FILL;
		break;
	case FILL_MODE_WIREFRAME:
		pRasInfo->polygonMode = VK_POLYGON_MODE_LINE;
		break;
	}

	pRasInfo->depthBiasEnable = false;
	pRasInfo->depthBiasConstantFactor = 0.0f;
	pRasInfo->depthBiasClamp = 0.0f;
	pRasInfo->depthBiasSlopeFactor = 0.0f;
	pRasInfo->lineWidth = pInfo->rasInfo.lineWidth;
}

void VulkanCreateBindingDescription(const VertexInputInfo* const pVertexInputInfo, VkVertexInputBindingDescription* pDesc)
{
	pDesc->binding = pVertexInputInfo->vertexBinding.binding;
	pDesc->stride = pVertexInputInfo->vertexBinding.stride;

	switch (pVertexInputInfo->vertexBinding.inputRate)
	{
	case INPUT_RATE_PER_VERTEX:
		pDesc->inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		break;

	case INPUT_RATE_PER_INSTANCE:
		pDesc->inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;
		break;
	}
}

void VulkanCreateAttributeDescription(const VertexInputInfo* const pVertexInputInfo, VkVertexInputAttributeDescription* pDesc)
{
	for (uint32_t i = 0; i < pVertexInputInfo->numVertexAttributes; ++i)
	{
		VkVertexInputAttributeDescription attribDesc{};
		attribDesc.binding = pVertexInputInfo->vertexAttributes[i].binding;
		attribDesc.location = pVertexInputInfo->vertexAttributes[i].location;
		attribDesc.format = (VkFormat)TinyImageFormat_ToVkFormat(pVertexInputInfo->vertexAttributes[i].format);
		attribDesc.offset = pVertexInputInfo->vertexAttributes[i].offset;

		pDesc[i] = attribDesc;
	}
}

void VulkanCreateVertexDescriptions(const PipelineInfo* const pInfo,
	VkVertexInputBindingDescription* pBindingDesc, VkVertexInputAttributeDescription* pAttribDesc,
	VkPipelineVertexInputStateCreateInfo* pCreateInfo)
{
	VulkanCreateBindingDescription(pInfo->pVertexInputInfo, pBindingDesc);

	VulkanCreateAttributeDescription(pInfo->pVertexInputInfo, pAttribDesc);

	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	pCreateInfo->sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	pCreateInfo->vertexBindingDescriptionCount = 1;
	pCreateInfo->pVertexBindingDescriptions = pBindingDesc;
	pCreateInfo->vertexAttributeDescriptionCount = pInfo->pVertexInputInfo->numVertexAttributes;
	pCreateInfo->pVertexAttributeDescriptions = pAttribDesc;
}

void VulkanCreateBlendInfo(const PipelineInfo* const pInfo, VkPipelineColorBlendAttachmentState* pBlendState,
	VkPipelineColorBlendStateCreateInfo* pBlendStateCreateInfo)
{
	pBlendState->colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	pBlendState->blendEnable = pInfo->blendInfo.enableBlend;
	pBlendState->srcColorBlendFactor = (VkBlendFactor)pInfo->blendInfo.srcColorBlendFactor;
	pBlendState->dstColorBlendFactor = (VkBlendFactor)pInfo->blendInfo.dstColorBlendFactor;
	pBlendState->colorBlendOp = (VkBlendOp)pInfo->blendInfo.colorBlendOp;
	pBlendState->srcAlphaBlendFactor = (VkBlendFactor)pInfo->blendInfo.srcAlphaBlendFactor;
	pBlendState->dstAlphaBlendFactor = (VkBlendFactor)pInfo->blendInfo.dstAlphaBlendFactor;
	pBlendState->alphaBlendOp = (VkBlendOp)pInfo->blendInfo.alphaBlendOp;
	pBlendState->colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT
		| VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

	pBlendStateCreateInfo->sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	pBlendStateCreateInfo->logicOpEnable = false;
	pBlendStateCreateInfo->logicOp = VK_LOGIC_OP_COPY;
	pBlendStateCreateInfo->attachmentCount = 1;
	pBlendStateCreateInfo->pAttachments = pBlendState;
	pBlendStateCreateInfo->blendConstants[0] = 1.0f;
	pBlendStateCreateInfo->blendConstants[1] = 1.0f;
	pBlendStateCreateInfo->blendConstants[2] = 1.0f;
	pBlendStateCreateInfo->blendConstants[3] = 1.0f;
}

void VulkanCreateGraphicsPipeline(const Renderer* const pRenderer, const PipelineInfo* const pInfo, Pipeline* pPipeline)
{
	VkVertexInputBindingDescription bindingDesc{};
	VkVertexInputAttributeDescription attribDesc[MAX_NUM_VERTEX_ATTRIBUTES]{};
	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	VulkanCreateVertexDescriptions(pInfo, &bindingDesc, attribDesc, &vertexInputInfo);

	VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};
	VulkanCreateInputAssemblyInfo(pInfo, &inputAssemblyInfo);

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
	VulkanCreateRasterizerInfo(pInfo, &rasInfo);

	VkPipelineMultisampleStateCreateInfo multisampling{};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = false;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampling.minSampleShading = 1.0f;
	multisampling.pSampleMask = nullptr;
	multisampling.alphaToCoverageEnable = false;
	multisampling.alphaToOneEnable = false;

	VkPipelineColorBlendAttachmentState blendState{};
	VkPipelineColorBlendStateCreateInfo blendStateCreateInfo{};
	VulkanCreateBlendInfo(pInfo, &blendState, &blendStateCreateInfo);

	VkPipelineDepthStencilStateCreateInfo depthStencil{};
	depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencil.pNext = nullptr;
	depthStencil.flags = 0;
	depthStencil.depthTestEnable = pInfo->depthInfo.depthTestEnable;
	depthStencil.depthWriteEnable = pInfo->depthInfo.depthWriteEnable;
	depthStencil.depthCompareOp = (VkCompareOp)pInfo->depthInfo.depthFunction;
	depthStencil.depthBoundsTestEnable = false;
	depthStencil.stencilTestEnable = false;
	depthStencil.front = {};
	depthStencil.back = {};
	depthStencil.minDepthBounds = 0.0f;
	depthStencil.maxDepthBounds = 1.0f;

	VkPipelineShaderStageCreateInfo shaders[] = { pInfo->pVertexShader->vk.shaderCreateInfo, pInfo->pPixelShader->vk.shaderCreateInfo };

	VkPipelineRenderingCreateInfo pipelineRendingerCreateInfo{};
	pipelineRendingerCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR;
	pipelineRendingerCreateInfo.pNext = nullptr;
	pipelineRendingerCreateInfo.viewMask = 0;
	pipelineRendingerCreateInfo.colorAttachmentCount = 1;
	VkFormat colorFormat = (VkFormat)TinyImageFormat_ToVkFormat(pInfo->renderTargetFormat);
	pipelineRendingerCreateInfo.pColorAttachmentFormats = &colorFormat;

	if (pInfo->depthInfo.depthTestEnable)
	{
		pipelineRendingerCreateInfo.depthAttachmentFormat = (VkFormat)TinyImageFormat_ToVkFormat(pInfo->depthFormat);
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
	pipelineInfo.layout = pInfo->pRootSignature->vk.pipelineLayout;
	pipelineInfo.renderPass = nullptr;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = nullptr;
	pipelineInfo.basePipelineIndex = -1;

	VULKAN_ERROR_CHECK(vkCreateGraphicsPipelines(pRenderer->vk.logicalDevice, nullptr, 1, &pipelineInfo, nullptr, &pPipeline->vk.pipeline));
}

void VulkanCreateComputePipeline(const Renderer* const pRenderer, const PipelineInfo* const pInfo, Pipeline* pPipeline)
{
	VkComputePipelineCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.flags = 0;
	createInfo.stage = pInfo->pComputeShader->vk.shaderCreateInfo;
	createInfo.layout = pInfo->pRootSignature->vk.pipelineLayout;
	createInfo.basePipelineHandle = nullptr;
	createInfo.basePipelineIndex = 0;

	VULKAN_ERROR_CHECK(vkCreateComputePipelines(pRenderer->vk.logicalDevice, nullptr, 1, &createInfo, nullptr, &pPipeline->vk.pipeline));
}

void VulkanCreatePipeline(const Renderer* const pRenderer, const PipelineInfo* const pInfo, Pipeline* pPipeline)
{
	if (pInfo->type == PIPELINE_TYPE_GRAPHICS)
	{
		VulkanCreateGraphicsPipeline(pRenderer, pInfo, pPipeline);
	}
	else //PIPELINE_TYPE_COMPUTE
	{
		VulkanCreateComputePipeline(pRenderer, pInfo, pPipeline);
	}

	pPipeline->pRootSignature = pInfo->pRootSignature;
	pPipeline->type = pInfo->type;
	pInfo->pRootSignature->pipelineType = pInfo->type;
}

void VulkanDestroyPipeline(const Renderer* const pRenderer, Pipeline* pPipeline)
{
	vkDestroyPipeline(pRenderer->vk.logicalDevice, pPipeline->vk.pipeline, nullptr);
}

//-------------------------------------------------------------------------------------------------------------------------------------------

//DRAW
//--------------------------------------------------------------------------------------------------------------------------------------------
void VulkanBindRenderTarget(CommandBuffer* pCommandBuffer, const BindRenderTargetInfo* const pInfo)
{
	if (pInfo == nullptr)
	{
		if (pCommandBuffer->isRendering)
		{
			vkCmdEndRendering(pCommandBuffer->vk.commandBuffer);
			pCommandBuffer->isRendering = false;
		}

		return;
	}

	VkRenderingAttachmentInfoKHR colorAttachment{};
	colorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
	colorAttachment.pNext = nullptr;
	colorAttachment.imageView = pInfo->pRenderTarget->vk.imageView;
	colorAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	colorAttachment.resolveMode = VK_RESOLVE_MODE_NONE;
	colorAttachment.loadOp = (VkAttachmentLoadOp)pInfo->depthTargetLoadOp;
	colorAttachment.storeOp = (VkAttachmentStoreOp)pInfo->depthTargetStoreOp;

	VkClearValue clearValue{};
	clearValue.color = { {pInfo->pRenderTarget->info.clearValue.r, pInfo->pRenderTarget->info.clearValue.g,
		pInfo->pRenderTarget->info.clearValue.b, pInfo->pRenderTarget->info.clearValue.a} };
	colorAttachment.clearValue = clearValue;

	VkRenderingAttachmentInfoKHR depthAttachment{};
	if (pInfo->pDepthTarget)
	{
		depthAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
		depthAttachment.pNext = nullptr;
		depthAttachment.imageView = pInfo->pDepthTarget->vk.imageView;
		depthAttachment.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		depthAttachment.resolveMode = VK_RESOLVE_MODE_NONE;
		depthAttachment.loadOp = (VkAttachmentLoadOp)pInfo->depthTargetLoadOp;
		depthAttachment.storeOp = (VkAttachmentStoreOp)pInfo->depthTargetStoreOp;

		VkClearValue clearValue{};
		clearValue.depthStencil = { pInfo->pDepthTarget->info.clearValue.depth, pInfo->pDepthTarget->info.clearValue.stencil};
		depthAttachment.clearValue = clearValue;
	}

	VkRenderingInfoKHR renderingInfo{};
	renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR;
	renderingInfo.pNext = nullptr;
	renderingInfo.flags = 0;
	renderingInfo.renderArea.offset = { 0, 0 };
	renderingInfo.renderArea.extent = { pInfo->pRenderTarget->info.width, pInfo->pRenderTarget->info.height };
	renderingInfo.layerCount = 1;
	renderingInfo.viewMask = 0;
	renderingInfo.colorAttachmentCount = 1;
	renderingInfo.pColorAttachments = &colorAttachment;
	renderingInfo.pDepthAttachment = (pInfo->pDepthTarget) ? &depthAttachment : nullptr;
	renderingInfo.pStencilAttachment = nullptr;

	vkCmdBeginRendering(pCommandBuffer->vk.commandBuffer, &renderingInfo);
	pCommandBuffer->isRendering = true;
}

void VulkanBindPipeline(CommandBuffer* pCommandBuffer, const Pipeline* const pPipeline)
{
	switch (pPipeline->type)
	{
	case PIPELINE_TYPE_GRAPHICS:
		vkCmdBindPipeline(pCommandBuffer->vk.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pPipeline->vk.pipeline);
		break;

	case PIPELINE_TYPE_COMPUTE:
		vkCmdBindPipeline(pCommandBuffer->vk.commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pPipeline->vk.pipeline);
		break;
	}

	pCommandBuffer->pCurrentPipeline = pPipeline;
}

void VulkanSetViewport(const CommandBuffer* const pCommandBuffer, const ViewportInfo* const pInfo)
{
	VkViewport viewport{};
	viewport.x = pInfo->x;
	viewport.y = pInfo->y;
	viewport.width = pInfo->width;
	viewport.height = pInfo->height;
	viewport.minDepth = pInfo->minDepth;
	viewport.maxDepth = pInfo->maxDepth;
	vkCmdSetViewport(pCommandBuffer->vk.commandBuffer, 0, 1, &viewport);
}

void VulkanSetScissor(const CommandBuffer* const pCommandBuffer, const ScissorInfo* const pInfo)
{
	VkRect2D scissor{};
	scissor.offset.x = pInfo->x;
	scissor.offset.y = pInfo->y;
	scissor.extent.width = pInfo->width;
	scissor.extent.height = pInfo->height;
	vkCmdSetScissor(pCommandBuffer->vk.commandBuffer, 0, 1, &scissor);
}

void VulkanDraw(const CommandBuffer* const pCommandBuffer, const uint32_t vertexCount,
	const uint32_t instanceCount, const uint32_t firstVertex, const uint32_t firstInstance)
{
	vkCmdDraw(pCommandBuffer->vk.commandBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
}

void VulkanDrawIndexed(const CommandBuffer* const pCommandBuffer, const uint32_t indexCount,
	const uint32_t instanceCount, const uint32_t firstIndex, const uint32_t vertexOffset, const uint32_t firstInstance)
{
	vkCmdDrawIndexed(pCommandBuffer->vk.commandBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}

void VulkanDispatch(const CommandBuffer* const pCommandBuffer, const uint32_t x, const uint32_t y, const uint32_t z)
{
	vkCmdDispatch(pCommandBuffer->vk.commandBuffer, x, y, z);
}

/*void VulkanEndCommandBuffer(const VulkanCommandBuffer* const commandBuffer)
{
	VULKAN_ERROR_CHECK(vkEndCommandBuffer(commandBuffer->commandBuffer));
}*/
//-------------------------------------------------------------------------------------------------------------------------------------------

//BUFFERS
//-------------------------------------------------------------------------------------------------------------------------------------------
void VulkanCreateStagingBuffer(const Renderer* const pRenderer, const uint32_t size)
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

	VmaAllocationCreateInfo allocationInfo{};
	allocationInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

	VULKAN_ERROR_CHECK(vkCreateBuffer(pRenderer->vk.logicalDevice, &bufferCreateInfo, nullptr, &gVulkanCopyEngine.buffer.vk.buffer));
	VULKAN_ERROR_CHECK(vmaAllocateMemoryForBuffer(pRenderer->vk.allocator, gVulkanCopyEngine.buffer.vk.buffer, 
		&allocationInfo, &gVulkanCopyEngine.buffer.vk.allocation, nullptr));

	VULKAN_ERROR_CHECK(vmaBindBufferMemory2(pRenderer->vk.allocator, gVulkanCopyEngine.buffer.vk.allocation, 
		0, gVulkanCopyEngine.buffer.vk.buffer, nullptr));

	gVulkanCopyEngine.buffer.size = size;
}

void VulkanDestroyStagingBuffer(const Renderer* const pRenderer)
{
	vkDestroyBuffer(pRenderer->vk.logicalDevice, gVulkanCopyEngine.buffer.vk.buffer, nullptr);
	vmaFreeMemory(pRenderer->vk.allocator, gVulkanCopyEngine.buffer.vk.allocation);
}

void VulkanCopyBuffer(const Renderer* const pRenderer, void* srcData, Buffer* dstBuffer, const BufferInfo* const pBufferInfo)
{
	VulkanCreateStagingBuffer(pRenderer, pBufferInfo->size);

	void* data = nullptr;
	vmaMapMemory(pRenderer->vk.allocator, gVulkanCopyEngine.buffer.vk.allocation, &data);
	memcpy(data, srcData, pBufferInfo->size);
	vmaUnmapMemory(pRenderer->vk.allocator, gVulkanCopyEngine.buffer.vk.allocation);

	VulkanBeginCopyEngineCommandBuffer(pRenderer);

	BarrierInfo barrier[2]{};
	barrier[0].type = BARRIER_TYPE_BUFFER;
	barrier[0].pBuffer = &gVulkanCopyEngine.buffer;
	barrier[0].currentState = RESOURCE_STATE_UNDEFINED;
	barrier[0].newState = RESOURCE_STATE_COPY_SOURCE;

	barrier[1].type = BARRIER_TYPE_BUFFER;
	barrier[1].pBuffer = dstBuffer;
	barrier[1].currentState = RESOURCE_STATE_UNDEFINED;
	barrier[1].newState = RESOURCE_STATE_COPY_DEST;
	VulkanResourceBarrier(&gVulkanCopyEngine.commandBuffer, 2, barrier);

	VkBufferCopy copyRegion{};
	copyRegion.srcOffset = 0;
	copyRegion.dstOffset = 0;
	copyRegion.size = pBufferInfo->size;
	vkCmdCopyBuffer(gVulkanCopyEngine.commandBuffer.vk.commandBuffer, gVulkanCopyEngine.buffer.vk.buffer, dstBuffer->vk.buffer, 1, &copyRegion);

	VulkanEndCopyEngineCommandBuffer(pRenderer);

	barrier[0].type = BARRIER_TYPE_BUFFER;
	barrier[0].pBuffer = dstBuffer;
	barrier[0].currentState = RESOURCE_STATE_COPY_DEST;
	barrier[0].newState = pBufferInfo->initialState;
	VulkanInitialTransition(pRenderer, barrier);

	VulkanDestroyStagingBuffer(pRenderer);
}

void VulkanCreateBuffer(const Renderer* const pRenderer, const BufferInfo* const pBufferInfo, Buffer* pBuffer)
{
	bool copyData = false;
	if (pBufferInfo->data && pBufferInfo->usage == MEMORY_USAGE_GPU_ONLY)
	{
		copyData = true;
	}

	VkBufferCreateInfo bufferCreateInfo{};
	bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferCreateInfo.pNext = nullptr;
	bufferCreateInfo.flags = 0;
	bufferCreateInfo.size = pBufferInfo->size;

	if (pBufferInfo->type & BUFFER_TYPE_VERTEX)
		bufferCreateInfo.usage |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

	if (pBufferInfo->type & BUFFER_TYPE_INDEX)
		bufferCreateInfo.usage |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
	
	if (pBufferInfo->type & BUFFER_TYPE_UNIFORM)
		bufferCreateInfo.usage |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

	if (pBufferInfo->type & BUFFER_TYPE_BUFFER)
		bufferCreateInfo.usage |= VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT;

	if (pBufferInfo->type & BUFFER_TYPE_RW_BUFFER)
		bufferCreateInfo.usage |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;

	if(copyData)
		bufferCreateInfo.usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;

	bufferCreateInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
	bufferCreateInfo.queueFamilyIndexCount = 3;
	bufferCreateInfo.pQueueFamilyIndices = pRenderer->vk.familyIndices;

	VmaAllocationCreateInfo allocationInfo{};
	switch (pBufferInfo->usage)
	{
	case MEMORY_USAGE_GPU_ONLY:
		allocationInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
		break;

	case MEMORY_USAGE_CPU_ONLY:
		allocationInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
		break;

	case MEMORY_USAGE_CPU_TO_GPU:
		allocationInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
		break;

	case MEMORY_USAGE_GPU_TO_CPU:
		allocationInfo.usage = VMA_MEMORY_USAGE_GPU_TO_CPU;
		break;
	}

	VULKAN_ERROR_CHECK(vkCreateBuffer(pRenderer->vk.logicalDevice, &bufferCreateInfo, nullptr, &pBuffer->vk.buffer));
	VULKAN_ERROR_CHECK(vmaAllocateMemoryForBuffer(pRenderer->vk.allocator, pBuffer->vk.buffer, &allocationInfo, &pBuffer->vk.allocation, nullptr));
	VULKAN_ERROR_CHECK(vmaBindBufferMemory2(pRenderer->vk.allocator, pBuffer->vk.allocation, 0, pBuffer->vk.buffer, nullptr));

	pBuffer->size = pBufferInfo->size;

	if (copyData)
	{
		VulkanCopyBuffer(pRenderer, pBufferInfo->data, pBuffer, pBufferInfo);
	}
	else
	{
		BarrierInfo barrier{};
		barrier.type = BARRIER_TYPE_BUFFER;
		barrier.pBuffer = pBuffer;
		barrier.currentState = RESOURCE_STATE_UNDEFINED;
		barrier.newState = pBufferInfo->initialState;
		VulkanInitialTransition(pRenderer, &barrier);
	}
}

void VulkanDestroyBuffer(const Renderer* const pRenderer, Buffer* pBuffer)
{
	vkDestroyBuffer(pRenderer->vk.logicalDevice, pBuffer->vk.buffer, nullptr);
	vmaFreeMemory(pRenderer->vk.allocator, pBuffer->vk.allocation);
}

void VulkanMapMemory(const Renderer* const pRenderer, const Buffer* const pBuffer, void** ppData)
{
	VULKAN_ERROR_CHECK(vmaMapMemory(pRenderer->vk.allocator, pBuffer->vk.allocation, ppData));
}

void VulkanUnmapMemory(const Renderer* const pRenderer, const Buffer* const pBuffer)
{
	vmaUnmapMemory(pRenderer->vk.allocator, pBuffer->vk.allocation);
}

void VulkanBindVertexBuffer(const CommandBuffer* const pCommandBuffer, const uint32_t stride,
	const uint32_t bindingLocation, const uint32_t offset, const Buffer* const pBuffer)
{
	VkDeviceSize offs = offset;
	vkCmdBindVertexBuffers(pCommandBuffer->vk.commandBuffer, bindingLocation, 1, &pBuffer->vk.buffer, &offs);
}

void VulkanBindIndexBuffer(const CommandBuffer* const pCommandBuffer, const uint32_t offset, const IndexType indexType, const Buffer* const pBuffer)
{
	vkCmdBindIndexBuffer(pCommandBuffer->vk.commandBuffer, pBuffer->vk.buffer, offset, (VkIndexType)indexType);
}
//-------------------------------------------------------------------------------------------------------------------------------------------

//DESCRIPTORS
//-------------------------------------------------------------------------------------------------------------------------------------------
void VulkanCreateDescriptorSet(const Renderer* const pRenderer, const DescriptorSetInfo* const pInfo,DescriptorSet* pDescriptorSet)
{
	pDescriptorSet->vk.pDescriptorSets = (VkDescriptorSet*)calloc(pInfo->numSets, sizeof(VkDescriptorSet));
	VkDescriptorSetLayout* layouts = (VkDescriptorSetLayout*)calloc(pInfo->numSets, sizeof(VkDescriptorSetLayout));

	for (uint32_t i = 0; i < pInfo->numSets; ++i)
	{
		layouts[i] = pInfo->pRootSignature->vk.descriptorSetLayouts[pInfo->updateFrequency];
	}

	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.pNext = nullptr;
	allocInfo.descriptorPool = gDescriptorPool;
	allocInfo.descriptorSetCount = pInfo->numSets;
	allocInfo.pSetLayouts = layouts;

	VULKAN_ERROR_CHECK(vkAllocateDescriptorSets(pRenderer->vk.logicalDevice, &allocInfo, pDescriptorSet->vk.pDescriptorSets));
	free(layouts);

	pDescriptorSet->updateFrequency = pInfo->updateFrequency;
	pDescriptorSet->pRootSignature = pInfo->pRootSignature;
}

void VulkanDestroyDescriptorSet(DescriptorSet* pDescriptorSet)
{
	free(pDescriptorSet->vk.pDescriptorSets);
}

void VulkanUpdateDescriptorSet(const Renderer* const pRenderer, const DescriptorSet* const  pDescriptorSet,
	const uint32_t index, const uint32_t numInfos, const UpdateDescriptorSetInfo* const pInfos)
{

	VkWriteDescriptorSet* descriptorWrites = (VkWriteDescriptorSet*)calloc(numInfos, sizeof(VkWriteDescriptorSet));
	uint32_t numBufferInfos = 0;
	uint32_t numImageInfos = 0;
	for (uint32_t i = 0; i < numInfos; ++i)
	{
		if (pInfos[i].type == UPDATE_TYPE_BUFFER || pInfos[i].type == UPDATE_TYPE_RW_BUFFER || pInfos[i].type == UPDATE_TYPE_UNIFORM_BUFFER)
		{
			++numBufferInfos;
		}
		else //UPDATE_TYPE_TEXTURE || UPDATE_TYPE_RW_TEXTURE || UPDATE_TYPE_SAMPLER
		{
			++numImageInfos;
		}
	}

	VkDescriptorBufferInfo* bufferInfos = (numBufferInfos > 0) ? (VkDescriptorBufferInfo*)calloc(numBufferInfos, sizeof(VkDescriptorBufferInfo)) : nullptr;
	uint32_t bufferInfoCount = 0;

	VkDescriptorImageInfo* imageInfos = (numImageInfos > 0) ? (VkDescriptorImageInfo*)calloc(numImageInfos, sizeof(VkDescriptorImageInfo)) : nullptr;
	uint32_t imageInfoCount = 0;

	for (uint32_t i = 0; i < numInfos; ++i)
	{
		if (pInfos[i].type == UPDATE_TYPE_BUFFER || pInfos[i].type == UPDATE_TYPE_RW_BUFFER || pInfos[i].type == UPDATE_TYPE_UNIFORM_BUFFER)
		{
			bufferInfos[bufferInfoCount].buffer = pInfos[i].pBuffer->vk.buffer;
			bufferInfos[bufferInfoCount].offset = 0;
			bufferInfos[bufferInfoCount].range = pInfos[i].pBuffer->size;

			descriptorWrites[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[i].pNext = nullptr;
			descriptorWrites[i].dstSet = pDescriptorSet->vk.pDescriptorSets[index];
			descriptorWrites[i].dstBinding = pInfos[i].binding;
			descriptorWrites[i].dstArrayElement = 0;
			descriptorWrites[i].descriptorCount = 1;
			descriptorWrites[i].pBufferInfo = &bufferInfos[bufferInfoCount];

			switch (pInfos[i].type)
			{
			case UPDATE_TYPE_UNIFORM_BUFFER:
				descriptorWrites[i].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				break;

			case UPDATE_TYPE_RW_BUFFER:
				descriptorWrites[i].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
				break;

			case UPDATE_TYPE_BUFFER:
				descriptorWrites[i].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
				break;
			}

			++bufferInfoCount;
		}
		else //UPDATE_TYPE_TEXTURE || UPDATE_TYPE_RW_TEXTURE || UPDATE_TYPE_SAMPLER
		{
			if (pInfos[i].type == UPDATE_TYPE_TEXTURE || pInfos[i].type == UPDATE_TYPE_RW_TEXTURE)
			{
				imageInfos[imageInfoCount].imageView = pInfos[i].pTexture->vk.imageView;
				imageInfos[imageInfoCount].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			}
			else //UPDATE_TYPE_SAMPLER
			{
				imageInfos[imageInfoCount].sampler = pInfos[i].pSampler->vk.sampler;
			}

			descriptorWrites[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[i].pNext = nullptr;
			descriptorWrites[i].dstSet = pDescriptorSet->vk.pDescriptorSets[index];
			descriptorWrites[i].dstBinding = pInfos[i].binding;
			descriptorWrites[i].dstArrayElement = 0;
			descriptorWrites[i].descriptorCount = 1;
			descriptorWrites[i].pImageInfo = &imageInfos[imageInfoCount];

			switch (pInfos[i].type)
			{
			case UPDATE_TYPE_TEXTURE:
				descriptorWrites[i].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
				break;

			case UPDATE_TYPE_RW_TEXTURE:
				descriptorWrites[i].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
				break;

			case UPDATE_TYPE_SAMPLER:
				descriptorWrites[i].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
				break;
			}

			++imageInfoCount;
		}
	}

	vkUpdateDescriptorSets(pRenderer->vk.logicalDevice, numInfos, descriptorWrites, 0, nullptr);

	free(imageInfos);
	free(bufferInfos);
	free(descriptorWrites);
}

void VulkanBindDescriptorSet(const CommandBuffer* const pCommandBuffer,
	const uint32_t index, const uint32_t firstSet, const DescriptorSet* const pDescriptorSet)
{
	VkPipelineBindPoint bindPoint{};
	switch (pDescriptorSet->pRootSignature->pipelineType)
	{
	case PIPELINE_TYPE_GRAPHICS:
		bindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		break;

	case PIPELINE_TYPE_COMPUTE:
		bindPoint = VK_PIPELINE_BIND_POINT_COMPUTE;
		break;
	}

	vkCmdBindDescriptorSets(pCommandBuffer->vk.commandBuffer, bindPoint, pDescriptorSet->pRootSignature->vk.pipelineLayout,
		firstSet, 1, &pDescriptorSet->vk.pDescriptorSets[index], 0, nullptr);
}

void VulkanBindRootConstants(const CommandBuffer* const pCommandBuffer, uint32_t numValues, uint32_t stride, const void* pData, uint32_t offset)
{
	vkCmdPushConstants(pCommandBuffer->vk.commandBuffer, pCommandBuffer->pCurrentPipeline->pRootSignature->vk.pipelineLayout,
		pCommandBuffer->pCurrentPipeline->pRootSignature->vk.rootConstantStages, offset, numValues * stride, pData);
}
//-------------------------------------------------------------------------------------------------------------------------------------------

//TEXTURES AND SAMPLERS
//-------------------------------------------------------------------------------------------------------------------------------------------

void VulkanCopyBufferToImage(const Renderer* const pRenderer, const TextureDesc* const textureInfo, const Texture* const pTexture)
{
	VulkanBeginCopyEngineCommandBuffer(pRenderer);

	BarrierInfo barrier[2]{};
	barrier[0].type = BARRIER_TYPE_BUFFER;
	barrier[0].pBuffer = &gVulkanCopyEngine.buffer;
	barrier[0].currentState = RESOURCE_STATE_UNDEFINED;
	barrier[0].newState = RESOURCE_STATE_COPY_SOURCE;

	barrier[1].type = BARRIER_TYPE_TEXTURE;
	barrier[1].pTexture = pTexture;
	barrier[1].currentState = RESOURCE_STATE_UNDEFINED;
	barrier[1].newState = RESOURCE_STATE_COPY_DEST;
	VulkanResourceBarrier(&gVulkanCopyEngine.commandBuffer, 2, barrier);

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

	vkCmdCopyBufferToImage(gVulkanCopyEngine.commandBuffer.vk.commandBuffer,
		gVulkanCopyEngine.buffer.vk.buffer, pTexture->vk.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

	VulkanEndCopyEngineCommandBuffer(pRenderer);

	BarrierInfo texBarrier{};
	texBarrier.type = BARRIER_TYPE_TEXTURE;
	texBarrier.pTexture = pTexture;
	texBarrier.currentState = RESOURCE_STATE_COPY_DEST;
	texBarrier.newState = RESOURCE_STATE_ALL_SHADER_RESOURCE;
	VulkanInitialTransition(pRenderer, &texBarrier);
}

void VulkanCreateTexture(const Renderer* const pRenderer, const TextureInfo* pInfo, Texture* pTexture)
{
	TextureDesc texInfo{};
	VkImageCreateInfo createImageInfo{};
	if (pInfo->filename != nullptr)
	{
		uint8_t* bitData = nullptr;
		uint32_t numBytes = 0;
		DDS_HEADER ddsHeader{};
		DDS_HEADER_DXT10 ddsHeader10{};
		ReadDDSFile(pInfo->filename, &bitData, &numBytes, &ddsHeader, &ddsHeader10);

		int result = RetrieveTextureInfo(&ddsHeader, &ddsHeader10, bitData, numBytes, &texInfo);
		if (result != SE_SUCCESS)
		{
			MessageBox(nullptr, L"Error with function RetrieveTextureInfo in function VulkanCreateTexture. Exiting Program.",
				L"RetrieveTextureInfo Error", MB_OK);
			free(bitData);
			exit(5);
		}

		VulkanCreateStagingBuffer(pRenderer, numBytes);

		void* data = nullptr;
		VULKAN_ERROR_CHECK(vmaMapMemory(pRenderer->vk.allocator, gVulkanCopyEngine.buffer.vk.allocation, &data));

		uint32_t numImages = texInfo.arraySize * texInfo.mipCount;
		uint32_t offset = 0;
		uint8_t* srcData = (uint8_t*)data;
		for (uint32_t i = 0; i < numImages; ++i)
		{
			memcpy(srcData + offset, texInfo.images[i].data, texInfo.images[i].numBytes);
			offset += texInfo.images[i].numBytes;
		}

		vmaUnmapMemory(pRenderer->vk.allocator, gVulkanCopyEngine.buffer.vk.allocation);

		free(bitData);
		free(texInfo.images);

		createImageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		createImageInfo.pNext = nullptr;
		createImageInfo.flags = (texInfo.isCubeMap) ? VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT : 0;

		switch (texInfo.resDim)
		{
		case TEXTURE_DIMENSION_1D:
			createImageInfo.imageType = VK_IMAGE_TYPE_1D;
			break;
		case TEXTURE_DIMENSION_2D:
			createImageInfo.imageType = VK_IMAGE_TYPE_2D;
			break;
		case TEXTURE_DIMENSION_3D:
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
		createImageInfo.pQueueFamilyIndices = pRenderer->vk.familyIndices;
		createImageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

		VmaAllocationCreateInfo allocationInfo{};
		allocationInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

		VULKAN_ERROR_CHECK(vkCreateImage(pRenderer->vk.logicalDevice, &createImageInfo, nullptr, &pTexture->vk.image));
		VULKAN_ERROR_CHECK(vmaAllocateMemoryForImage(pRenderer->vk.allocator, pTexture->vk.image, &allocationInfo, &pTexture->vk.allocation, nullptr));
		VULKAN_ERROR_CHECK(vmaBindImageMemory2(pRenderer->vk.allocator, pTexture->vk.allocation, 0, pTexture->vk.image, nullptr));

		VulkanCopyBufferToImage(pRenderer, &texInfo, pTexture);

		VulkanDestroyStagingBuffer(pRenderer);
	}
	else
	{
		createImageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		createImageInfo.pNext = nullptr;
		createImageInfo.flags = 0;

		switch (pInfo->dimension)
		{
		case TEXTURE_DIMENSION_1D:
			createImageInfo.imageType = VK_IMAGE_TYPE_1D;
			break;
		case TEXTURE_DIMENSION_2D:
			createImageInfo.imageType = VK_IMAGE_TYPE_2D;
			break;
		case TEXTURE_DIMENSION_3D:
			createImageInfo.imageType = VK_IMAGE_TYPE_3D;
			break;
		}

		createImageInfo.format = (VkFormat)TinyImageFormat_ToVkFormat(pInfo->format);
		createImageInfo.extent.width = pInfo->width;
		createImageInfo.extent.height = pInfo->height;
		createImageInfo.extent.depth = pInfo->depth;
		createImageInfo.mipLevels = pInfo->mipCount;
		createImageInfo.arrayLayers = pInfo->arraySize;
		createImageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		createImageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		createImageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		createImageInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
		createImageInfo.queueFamilyIndexCount = 3;
		createImageInfo.pQueueFamilyIndices = pRenderer->vk.familyIndices;
		createImageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

		VmaAllocationCreateInfo allocationInfo{};
		allocationInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

		VULKAN_ERROR_CHECK(vkCreateImage(pRenderer->vk.logicalDevice, &createImageInfo, nullptr, &pTexture->vk.image));
		VULKAN_ERROR_CHECK(vmaAllocateMemoryForImage(pRenderer->vk.allocator, pTexture->vk.image, &allocationInfo, &pTexture->vk.allocation, nullptr));
		VULKAN_ERROR_CHECK(vmaBindImageMemory2(pRenderer->vk.allocator, pTexture->vk.allocation, 0, pTexture->vk.image, nullptr));

		//TRANSITION TO INITIAL STATE
		BarrierInfo barrier{};
		barrier.type = BARRIER_TYPE_TEXTURE;
		barrier.pTexture = pTexture;
		barrier.currentState = RESOURCE_STATE_UNDEFINED;
		barrier.newState = pInfo->initialState;
		VulkanInitialTransition(pRenderer, &barrier);
	}

	VkImageViewCreateInfo createImageViewInfo{};
	createImageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	createImageViewInfo.pNext = nullptr;
	createImageViewInfo.flags = 0;
	createImageViewInfo.image = pTexture->vk.image;
	if (createImageInfo.imageType == VK_IMAGE_TYPE_1D)
	{
		if (createImageInfo.arrayLayers > 1)
		{
			createImageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_1D_ARRAY;
		}
		else
		{
			createImageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_1D;
		}
	}
	else if (createImageInfo.imageType == VK_IMAGE_TYPE_2D)
	{
		if (texInfo.isCubeMap || pInfo->isCubeMap)
		{
			createImageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_CUBE;
		}
		else if (createImageInfo.arrayLayers > 1)
		{
			createImageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
		}
		else
		{
			createImageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		}
	}
	else if (createImageInfo.imageType == VK_IMAGE_TYPE_3D)
	{
		createImageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_3D;
	}
	createImageViewInfo.format = (VkFormat)TinyImageFormat_ToVkFormat(pInfo->filename == nullptr ? pInfo->format : texInfo.format);
	createImageViewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	createImageViewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	createImageViewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	createImageViewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
	createImageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	createImageViewInfo.subresourceRange.baseMipLevel = 0;
	createImageViewInfo.subresourceRange.levelCount = createImageInfo.mipLevels;
	createImageViewInfo.subresourceRange.baseArrayLayer = 0;
	createImageViewInfo.subresourceRange.layerCount = createImageInfo.arrayLayers;

	VULKAN_ERROR_CHECK(vkCreateImageView(pRenderer->vk.logicalDevice, &createImageViewInfo, nullptr, &pTexture->vk.imageView));
}

void VulkanDestroyTexture(const Renderer* const pRenderer, Texture* pTexture)
{
	vkDestroyImageView(pRenderer->vk.logicalDevice, pTexture->vk.imageView, nullptr);
	vkDestroyImage(pRenderer->vk.logicalDevice, pTexture->vk.image, nullptr);
	vmaFreeMemory(pRenderer->vk.allocator, pTexture->vk.allocation);
}

VkFilter VulkanGetFilter(const Filter filter)
{
	switch (filter)
	{
	case FILTER_NEAREST:
		return VK_FILTER_NEAREST;

	case FILTER_LINEAR:
		return VK_FILTER_LINEAR;
	}
}

VkSamplerAddressMode VulkanGetAddressMode(AddressMode addressMode)
{
	switch (addressMode)
	{
	case ADDRESS_MODE_REPEAT:
		return VK_SAMPLER_ADDRESS_MODE_REPEAT;

	case ADDRESS_MODE_MIRRORED:
		return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;

	case ADDRESS_MODE_CLAMP_TO_EDGE:
		return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;

	case ADDRESS_MODE_CLAMP_TO_BORDER:
		return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
	}
}

VkSamplerMipmapMode VulkanGetMipmapMode(MipMapMode mode)
{
	switch (mode)
	{
	case MIPMAP_MODE_NEAREST:
		return VK_SAMPLER_MIPMAP_MODE_NEAREST;

	case MIPMAP_MODE_LINEAR:
		return VK_SAMPLER_MIPMAP_MODE_LINEAR;
	}
}

void VulkanCreateSampler(const Renderer* const pRenderer, const SamplerInfo* const pInfo, Sampler* pSampler)
{
	VkSamplerCreateInfo samplerInfo{};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.pNext = nullptr;
	samplerInfo.flags = 0;
	samplerInfo.magFilter = VulkanGetFilter(pInfo->magFilter);
	samplerInfo.minFilter = VulkanGetFilter(pInfo->minFilter);
	samplerInfo.addressModeU = VulkanGetAddressMode(pInfo->u);
	samplerInfo.addressModeV = VulkanGetAddressMode(pInfo->v);
	samplerInfo.addressModeW = VulkanGetAddressMode(pInfo->w);
	samplerInfo.anisotropyEnable = pInfo->maxAnisotropy > 0.0f ? true : false;
	samplerInfo.maxAnisotropy = pInfo->maxAnisotropy > gMaxSamplerAnisotropy ? gMaxSamplerAnisotropy : pInfo->maxAnisotropy;
	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerInfo.mipmapMode = VulkanGetMipmapMode(pInfo->mipMapMode);
	samplerInfo.mipLodBias = pInfo->mipLoadBias;
	samplerInfo.minLod = pInfo->minLod;
	samplerInfo.maxLod = pInfo->maxLod;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;

	VULKAN_ERROR_CHECK(vkCreateSampler(pRenderer->vk.logicalDevice, &samplerInfo, nullptr, &pSampler->vk.sampler));
}

void VulkanDestroySampler(const Renderer* const pRenderer, Sampler* pSampler)
{
	vkDestroySampler(pRenderer->vk.logicalDevice, pSampler->vk.sampler, nullptr);
}
//-------------------------------------------------------------------------------------------------------------------------------------------