#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>

#include <string>

#include "SEWindow.h"
#include "SEFileSystem.h"

inline std::wstring AnsiToWString(const std::string& str)
{
	WCHAR buffer[512];
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, buffer, 512);
	return std::wstring(buffer);
}
				
#ifndef ExitIfFailed
#define ExitIfFailed(x)																										\
{																															\
	VkResult result = (x);																									\
	if (result != VK_SUCCESS)																								\
	{																														\
		std::wstring resultString = AnsiToWString(string_VkResult(result));													\
		std::wstring functionName = L#x;																					\
		std::wstring errMsg = functionName + L"\nFailed in " + AnsiToWString(__FILE__) +									\
							  L"\nLine " + std::to_wstring(__LINE__) + L"\nError = " + resultString;						\
		MessageBox(nullptr, errMsg.c_str(), L"Vulkan Function Failed", MB_OK);												\
		ExitProcess(2);																										\
	}																														\
}																																
#endif

struct SEVulkan
{
	VkInstance instance;

#ifdef _DEBUG
	const bool enableValidationLayers = true;
#else
	const bool enableValidationLayers = false;
#endif

	VkDebugUtilsMessengerEXT debugMessenger;

	VkPhysicalDevice physicalDevice;

	uint32_t graphicsFamilyIndex;
	uint32_t computeFamilyIndex;

	VkDevice logicalDevice;

	VkSurfaceKHR surface;
};


void InitVulkan(SEVulkan* vulk, SEWindow* window, const char* appName);
void DestroyVulkan(SEVulkan* vulk);

enum SEVulkanQueueType
{
	GRAPHICS_QUEUE,
	COMPUTE_QUEUE
};

struct SEVulkanQueueInfo
{
	SEVulkanQueueType queueType;
	uint32_t index;
};

struct SEVulkanQueue
{
	VkQueue queue;
};

void GetQueueHandle(const SEVulkan* vulk, const SEVulkanQueueInfo* queueInfo, SEVulkanQueue* queue);

enum SEFormat
{
	SE_FORMAT_R8G8B8A8_SRGB,
	SE_FORMAT_R32_FLOAT,
	SE_FORMAT_R32G32_FLOAT,
	SE_FORMAT_R32G32B32_FLOAT,
	SE_FORMAT_R32G32B32A32_FLOAT
};

struct SEImageViewInfo
{
	VkImage image;
	VkImageViewType viewType;
	SEFormat format;
	uint32_t firstMipLevel;
	uint32_t numMipLevels;
	uint32_t firstArrayLayer;
	uint32_t numLayers;
};

struct SEVulkanImageView
{
	VkImageView imageView;
};

void CreateImageView(SEVulkan* vulk, SEImageViewInfo* info, SEVulkanImageView* imageView);
void DestroyImageView(SEVulkan* vulk, SEVulkanImageView* imageView);

struct SEVulkanSwapChain
{
	VkSwapchainKHR swapChain;
	uint32_t imageCount;
	VkImage* images;
	VkFormat format;
	VkExtent2D extent;
	SEVulkanImageView* imageViews;
	VkFramebuffer* frameBuffers;
};

void CreateVulkanSwapChain(SEVulkan* vulk, SEWindow* window, SEVulkanSwapChain* swapChain);
void DestroyVulkanSwapChain(SEVulkan* vulk, SEVulkanSwapChain* swapChain);

void CreateVulkanFrameBuffer(SEVulkan* vulk, SEVulkanSwapChain* swapChain, VkRenderPass* renderPass);
void DestroyVulkanFrameBuffer(SEVulkan* vulk, VkFramebuffer* frameBuffer);

enum ShaderType
{
	VERTEX,
	PIXEL,
	COMPUTE
};

struct SEVulkanShader
{
	VkShaderModule shaderModule;
	VkPipelineShaderStageCreateInfo shaderCreateInfo;
};

void CreateVulkanShader(SEVulkan* vulk, const char* filename, ShaderType type, SEVulkanShader* shader);
void DestroyVulkanShader(SEVulkan* vulk, SEVulkanShader* shader);

enum SETopology
{
	POINT_LIST,
	LINE_LIST,
	LINE_STRIP,
	TRIANGLE_LIST,
	TRIANGLE_STRIP
};

enum SECullMode
{
	NONE,
	FRONT,
	BACK,
};

enum SEFace
{
	CLOCKWISE,
	COUNTER_CLOCKWISE
};

enum SEVulkanInputRate
{
	SE_PER_VERTEX,
	SE_PER_INSTANCE
};

struct SEVulkanVertexBindingInfo
{
	uint32_t binding;
	uint32_t stride;
	SEVulkanInputRate inputRate;
};

struct SEVulkanAttributeInfo
{
	uint32_t binding;
	uint32_t location;
	SEFormat format;
	uint32_t offset;
};

struct SEVulkanPipleineInfo
{
	SETopology topology;

	struct RasterizerInfo
	{
		SECullMode cullMode;
		SEFace faceMode;
		float lineWidth;
	}rasInfo;

	SEVulkanShader shaders[2];

	VkFormat swapChainFormat;

	SEVulkanVertexBindingInfo bindingInfo;
	SEVulkanAttributeInfo* attributeInfo;
	uint32_t numAttributeInfos;
};

struct SEVulkanPipeline
{
	VkPipelineLayout pipelineLayout;
	VkRenderPass renderPass;
	VkPipeline pipeline;
};

enum SEPipelineType
{
	GRAPHICS_PIPELINE,
	COMPUTE_PIPELINE
		
};

void CreateVulkanPipeline(SEVulkan* vulk, SEVulkanPipleineInfo* info, SEVulkanPipeline* pipeline);
void DestroyVulkanPipeline(SEVulkan* vulk, SEVulkanPipeline* pipeline);

struct SEVulkanCommandPool
{
	VkCommandPool commandPool;
};

void CreateVulkanCommandPool(SEVulkan* vulk, SEVulkanCommandPool* commandPool, SEVulkanQueueType queueType);
void DestroyVulkanCommandPool(SEVulkan* vulk, SEVulkanCommandPool* commandPool);

struct SEVulkanCommandBuffer
{
	VkCommandBuffer commandBuffer;
};

void CreateVulkanCommandBuffer(SEVulkan* vulk, SEVulkanCommandPool* commandPool,
	SEVulkanCommandBuffer* commandBuffer, uint32_t numCommandBuffers);

void VulkanResetCommandBuffer(SEVulkanCommandBuffer* commandBuffer);

void VulkanBeginCommandBuffer(SEVulkan* vulk, SEVulkanCommandBuffer* commandBuffer, SEVulkanPipeline* pipeline,
	SEVulkanSwapChain* swapChain, uint32_t imageIndex);

void VulkanBindPipeline(SEVulkanCommandBuffer* commandBuffer, SEVulkanPipeline* pipeline, SEPipelineType type);

struct SEViewportInfo
{
	float x;
	float y;
	float width;
	float height;
	float minDepth;
	float maxDepth;
};

struct SEScissorInfo
{
	int32_t x;
	int32_t y;
	uint32_t width;
	uint32_t height;
};

void VulkanSetViewport(SEVulkanCommandBuffer* commandBuffer, SEViewportInfo* viewportInfo);
void VulkanSetScissor(SEVulkanCommandBuffer* commandBuffer, SEScissorInfo* scissorInfo);

void VulkanDraw(SEVulkanCommandBuffer* commandBuffer, uint32_t vertexCount,
	uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance);

void VulkanEndCommandBuffer(SEVulkanCommandBuffer* commandBuffer);


struct SEVulkanFence
{
	VkFence fence;
};

void CreateVulkanFence(SEVulkan* vulk, SEVulkanFence* fence);

void DestroyVulkanFence(SEVulkan* vulk, SEVulkanFence* fence);

void VulkanWaitForFence(SEVulkan* vulk, SEVulkanFence* fence);

struct SEVulkanSemaphore
{
	VkSemaphore semaphore;
};

void CreateVulkanSemaphore(SEVulkan* vulk, SEVulkanSemaphore* semaphore);

void DestroyVulkanSemaphore(SEVulkan* vulk, SEVulkanSemaphore* semaphore);

void VulkanAcquireNextImage(SEVulkan* vulk, SEVulkanSwapChain* swapChain, 
	SEVulkanSemaphore* imageAvailableSemaphore, uint32_t* imageIndex);


struct VulkanQueueSubmitInfo
{
	SEVulkanQueue* queue;
	SEVulkanSemaphore* waitSemaphore;	//wait for image to become available
	SEVulkanSemaphore* signalSemaphore; //signal for when the command buffer is finished executing
	SEVulkanFence* fence;
	SEVulkanCommandBuffer* commandBuffer;
};

void VulkanQueueSubmit(SEVulkan* vulk, VulkanQueueSubmitInfo* info);


struct VulkanPresentInfo
{
	SEVulkanQueue* queue;
	SEVulkanSemaphore* waitSemaphore;	//wait for the command buffer to finish execution
	SEVulkanSwapChain* swapChain;
	uint32_t imageIndex;
};

void VulkanQueuePresent(SEVulkan* vulk, VulkanPresentInfo* info);

void VulkanWaitDeviceIdle(SEVulkan* vulk);

void VulkanWaitQueueIdle(SEVulkanQueue* queue);

void VulkanOnResize(SEVulkan* vulk, SEWindow* window, SEVulkanSwapChain* swapChain, SEVulkanPipeline* pipeline);

enum SEVulkanBufferUsageFlags
{
	VERTEX_BUFFER
};

struct SEVulkanBufferInfo
{
	uint32_t size;
	SEVulkanBufferUsageFlags usage;

};

struct SEVulkanBuffer
{
	VkBuffer buffer;
};

void CreateVulkanBuffer(SEVulkan* vulk, SEVulkanBufferInfo* bufferInfo, SEVulkanBuffer* buffer);

void DestroyVulkanBuffer(SEVulkan* vulk, SEVulkanBuffer* buffer);