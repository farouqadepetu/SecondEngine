#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>

#include "SEWindow.h"
#include "SEFileSystem.h"

#include "vma/vk_mem_alloc.h"

#include "tinyimageformat_apis.h"
				
/*#ifndef VULKAN_ERROR_CHECK
#define VULKAN_ERROR_CHECK(x)																								\
{																															\
	VkResult result = (x);																									\
	if (result != VK_SUCCESS)																								\
	{																														\
		std::wstring resultString = AnsiToWString(string_VkResult(result));													\
		std::wstring functionName = L#x;																					\
		std::wstring errMsg = functionName + L"\nFailed in " + AnsiToWString(__FILE__) +									\
							  L"\nLine " + std::to_wstring(__LINE__) + L"\nError = " + resultString;						\
		MessageBox(nullptr, errMsg.c_str(), L"Vulkan Function Failed", MB_OK);												\
		exit(2);																											\
	}																														\
}																																
#endif*/

#ifndef VULKAN_ERROR_CHECK
#define VULKAN_ERROR_CHECK(x)																								\
{																															\
	VkResult result = (x);																									\
	if (result != VK_SUCCESS)																								\
	{																														\
		char errMsg[2048]{};																								\
		sprintf_s(errMsg, "%s\nFailed in %s\nLine %d\n Error = %s", #x, __FILE__, __LINE__, string_VkResult(result));		\
		MessageBoxA(nullptr, errMsg, "Vulkan Function Failed", MB_OK);														\
		exit(2);																											\
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

	uint32_t familyIndices[3];

	VkDevice logicalDevice;

	VkSurfaceKHR surface;

	VmaAllocator allocator;
};


void InitVulkan(SEVulkan* vulk, SEWindow* window, const char* appName);
void DestroyVulkan(SEVulkan* vulk);

enum SEVulkanQueueType
{
	SE_GRAPHICS_QUEUE,
	SE_COMPUTE_QUEUE
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

struct SEImageViewInfo
{
	VkImage image;
	VkImageViewType viewType;
	TinyImageFormat format;
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

enum SEShaderType
{
	SE_VERTEX_SHADER,
	SE_PIXEL_SHADER,
	SE_COMPUTE_SHADER
};

struct SEVulkanShader
{
	VkShaderModule shaderModule;
	VkPipelineShaderStageCreateInfo shaderCreateInfo;
};

void CreateVulkanShader(SEVulkan* vulk, const char* filename, SEShaderType type, SEVulkanShader* shader);
void DestroyVulkanShader(SEVulkan* vulk, SEVulkanShader* shader);

enum SETopology
{
	SE_POINT_LIST,
	SE_LINE_LIST,
	SE_LINE_STRIP,
	SE_TRIANGLE_LIST,
	SE_TRIANGLE_STRIP
};

enum SECullMode
{
	SE_CULL_NONE,
	SE_CULL_FRONT,
	SE_CULL_BACK,
};

enum SEFace
{
	SE_FACE_CLOCKWISE,
	SE_FACE_COUNTER_CLOCKWISE
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

#define NUM_ATTRIBUTES 8

struct SEVulkanAttributeInfo
{
	uint32_t binding;
	uint32_t location;
	TinyImageFormat format;
	uint32_t offset;
};

#define MAX_NUM_DESCRIPTOR_SET_LAYOUT_BINDINGS 16
enum SEStages
{
	SE_VERTEX_STAGE = 0x00000001,
	SE_PIXEL_STAGE = 0x00000002
};

enum SEDescriptorType
{
	SE_DESCRITPTOR_UNIFORM_BUFFER
};

struct SEVulkanDescritporSetLayoutInfo
{
	struct DescritporSetLayoutBindingInfo
	{
		uint32_t binding;
		SEDescriptorType type;
		uint32_t numDescriptors;
		SEStages stages;
	}bindingInfo[MAX_NUM_DESCRIPTOR_SET_LAYOUT_BINDINGS];

	uint32_t numBindings;
};

struct SEVulkanDescriptorSetLayout
{
	VkDescriptorSetLayout descriptorSetLayout;
};

void CreateVulkanDescriptorSetLayout(SEVulkan* vulk, SEVulkanDescritporSetLayoutInfo* info, SEVulkanDescriptorSetLayout* dsl);

void DestroyVulkanDescriptorSetLayout(SEVulkan* vulk, SEVulkanDescriptorSetLayout* dsl);

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
	SEVulkanAttributeInfo attributeInfo[NUM_ATTRIBUTES];
	uint32_t numAttributes;
	SEVulkanDescriptorSetLayout* descriptorSetLayout;
};

struct SEVulkanPipeline
{
	VkPipelineLayout pipelineLayout;
	VkRenderPass renderPass;
	VkPipeline pipeline;
};

enum SEPipelineType
{
	SE_GRAPHICS_PIPELINE,
	SE_COMPUTE_PIPELINE	
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

void VulkanDrawIndexed(SEVulkanCommandBuffer* commandBuffer, uint32_t indexCount,
	uint32_t instanceCount, uint32_t firstIndex, uint32_t vertexOffset, uint32_t firstInstance);

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

enum SEVulkanBufferType
{
	SE_VERTEX_BUFFER,
	SE_INDEX_BUFFER,
	SE_UNIFORM_BUFFER
};

enum SEVulkanAccessFlags
{
	SE_GPU,
	SE_CPU_GPU
};

struct SEVulkanBufferInfo
{
	uint32_t size;
	SEVulkanBufferType type;
	SEVulkanAccessFlags access;

	//Will only copy data when creating the buffer if access = SE_GPU
	void* data;
};

struct SEVulkanBuffer
{
	VkBuffer buffer;
	VmaAllocation allocation;
};

void CreateVulkanBuffer(SEVulkan* vulk, SEVulkanBufferInfo* bufferInfo, SEVulkanBuffer* buffer);

void DestroyVulkanBuffer(SEVulkan* vulk, SEVulkanBuffer* buffer);

void VulkanMapMemory(SEVulkan* vulk, SEVulkanBuffer* buffer, uint32_t offset, uint32_t size, void** data);

void VulkanUnmapMemory(SEVulkan* vulk, SEVulkanBuffer* buffer);

void VulkanBindBuffer(SEVulkanCommandBuffer* commandBuffer, uint32_t bindingLocation,
	SEVulkanBuffer* buffer, uint32_t offset, SEVulkanBufferType bufferType);

enum SEVulkanDescriptorType
{
	SE_DESCRIPTOR_TYPE_UNIFORM
};

struct SEVulkanDescriptorSetInfo
{
	SEVulkanDescriptorSetLayout* layout;
	SEDescriptorType type;
	uint32_t numDescriptors;
};

struct SEVulkanUpdateDescriptorSetInfo
{
	uint32_t binding;
	uint32_t firstArrayElement;
	uint32_t numArrayElements;

	struct Buffer
	{
		SEVulkanBuffer* buffer;
		uint32_t offset;
		uint32_t range;
	}bufferInfo;
};

struct SEVulkanDescriptorSet
{
	VkDescriptorSet descriptorSet;
};

void CreateVulkanDescriptorSets(SEVulkan* vulk, SEVulkanDescriptorSetInfo* info, SEVulkanDescriptorSet* ds);
void UpdateVulkanDescriptorSet(SEVulkan* vulk, SEVulkanUpdateDescriptorSetInfo* info, SEVulkanDescriptorSet* ds);
void VulkanBindDescriptorSet(SEVulkanCommandBuffer* commandBuffer, SEPipelineType type, SEVulkanPipeline* pipeline, SEVulkanDescriptorSet* ds);