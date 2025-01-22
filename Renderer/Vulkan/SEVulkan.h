#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan\vulkan.h>
#include <vulkan\vk_enum_string_helper.h>

#include "Renderer\SEWindow.h"
#include "FileSystem\SEFileSystem.h"

#include "vma\vk_mem_alloc.h"

#include "ThirdParty\TinyImage\tinyimageformat_apis.h"

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

struct Vulkan
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


void VulkanInit(Vulkan* vulk, const Window* const window, const char* appName);
void VulkanDestroy(Vulkan* vulk);

enum VulkanQueueType
{
	QUEUE_TYPE_GRAPHICS,
	QUEUE_TYPE_COMPUTE
};

struct VulkanQueueInfo
{
	VulkanQueueType queueType;
	uint32_t index;
};

struct VulkanQueue
{
	VkQueue queue;
};

void VulkanGetQueueHandle(const Vulkan* const vulk, const VulkanQueueInfo* const queueInfo, VulkanQueue* queue);

union ClearValue
{
	struct
	{
		float r;
		float g;
		float b;
		float a;
	};
	struct
	{
		float depth;
		uint32_t stencil;
	};
};

struct VulkanRenderAttachmentInfo
{
	TinyImageFormat format;
	uint32_t width;
	uint32_t height;
	ClearValue clearValue;
};

struct VulkanRenderAttachment
{
	TinyImageFormat format;
	VkImage image;
	VkImageView imageView;
	VmaAllocation allocation;
	ClearValue clearValue;
	uint32_t width;
	uint32_t height;
};

void VulkanCreateRenderingAttachment(const Vulkan* const vulk, 
	const VulkanRenderAttachmentInfo* const info, VulkanRenderAttachment* attachment);

void VulkanDestroyRenderingAttachment(const Vulkan* const vulk, VulkanRenderAttachment* attachment);

struct VulkanSwapChainInfo
{
	Window* window;
	uint32_t width;
	uint32_t height;
	TinyImageFormat format;
	ClearValue clearValue;
};

struct VulkanSwapChain
{
	TinyImageFormat format;
	Window* window;
	VkSwapchainKHR swapChain;
	uint32_t imageCount;
	uint32_t width;
	uint32_t height;
	VulkanRenderAttachment* colorAttachments;
	ClearValue clearValue;
};

void VulkanCreateSwapChain(const Vulkan* const vulk, const VulkanSwapChainInfo* const info, VulkanSwapChain* swapChain);
void VulkanDestroySwapChain(const Vulkan* const vulk, VulkanSwapChain* swapChain);

enum VulkanShaderType
{
	SHADER_TYPE_VERTEX,
	SHADER_TYPE_PIXEL,
	SHADER_TYPE_COMPUTE
};

struct VulkanShaderInfo
{
	const char* filename;
	VulkanShaderType type;
};

struct VulkanShader
{
	VkShaderModule shaderModule;
	VkPipelineShaderStageCreateInfo shaderCreateInfo;
};

void VulkanCreateShader(const Vulkan* const vulk, const VulkanShaderInfo* const info, VulkanShader* shader);
void VulkanDestroyShader(const Vulkan* const vulk, VulkanShader* shader);

enum Topology
{
	TOPOLOGY_POINT_LIST,
	TOPOLOGY_LINE_LIST,
	TOPOLOGY_LINE_STRIP,
	TOPOLOGY_TRIANGLE_LIST,
	TOPOLOGY_TRIANGLE_STRIP
};

enum CullMode
{
	CULL_MODE_NONE,
	CULL_MODE_FRONT,
	CULL_MODE_BACK,
};

enum Face
{
	FACE_CLOCKWISE,
	FACE_COUNTER_CLOCKWISE
};

enum PolygonMode
{
	POLYGON_MODE_FILL,
	POLYGON_MODE_WIREFRAME
};

enum VulkanInputRate
{
	INPUT_RATE_PER_VERTEX,
	INPUT_RATE_PER_INSTANCE
};

struct VulkanVertexBindingInfo
{
	uint32_t binding;
	uint32_t stride;
	VulkanInputRate inputRate;
};

#define NUM_ATTRIBUTES 8

struct VulkanAttributeInfo
{
	uint32_t binding;
	uint32_t location;
	TinyImageFormat format;
	uint32_t offset;
};

#define MAX_NUM_DESCRIPTOR_SET_LAYOUT_BINDINGS 16
enum Stages
{
	STAGE_VERTEX = 0x00000001,
	STAGE_PIXEL = 0x00000002,
	STAGE_COMPUTE = 0x00000004
};

enum VulkanDescriptorType
{
	DESCRITPTOR_TYPE_UNIFORM_BUFFER,
	DESCRITPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
	DESCRITPTOR_TYPE_STORAGE_BUFFER
};

struct VulkanDescritporSetLayoutInfo
{
	struct DescritporSetLayoutBindingInfo
	{
		uint32_t binding;
		VulkanDescriptorType type;
		uint32_t numDescriptors;
		uint32_t stages;
	}bindingInfo[MAX_NUM_DESCRIPTOR_SET_LAYOUT_BINDINGS];

	uint32_t numBindings;
};

struct VulkanDescriptorSetLayout
{
	VkDescriptorSetLayout descriptorSetLayout;
};

void VulkanCreateDescriptorSetLayout(const Vulkan* const vulk, const VulkanDescritporSetLayoutInfo* const info, 
	VulkanDescriptorSetLayout* setLayout);

void VulkanDestroyDescriptorSetLayout(const Vulkan* const vulk, VulkanDescriptorSetLayout* setLayout);

enum PipelineType
{
	PIPELINE_TYPE_GRAPHICS,
	PIPELINE_TYPE_COMPUTE
};

enum BlendFactor
{
	BLEND_FACTOR_ZERO,
	BLEND_FACTOR_ONE,
	BLEND_FACTOR_SRC_COLOR,
	BLEND_FACTOR_ONE_MINUS_SRC_COLOR,
	BLEND_FACTOR_DST_COLOR,
	BLEND_FACTOR_ONE_MINUS_DST_COLOR,
	BLEND_FACTOR_SRC_ALPHA,
	BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
	BLEND_FACTOR_DST_ALPHA,
	BLEND_FACTOR_ONE_MINUS_DST_ALPHA,
	BLEND_FACTOR_CONST_COLOR,
	BLEND_FACTOR_ONE_MINUS_CONST_COLOR,
	BLEND_FACTOR_CONST_ALPHA,
	BLEND_FACTOR_ONE_MINUS_CONST_ALPHA
};

enum BlendOp
{
	BLEND_OP_ADD,
	BLEND_OP_SUBTRACT,
	BLEND_OP_REVERSE_SUBTRACT,
	BLEND_OP_MIN,
	BLEND_OP_MAX
};

enum DepthFunction
{
	DEPTH_FUNCTION_NEVER,
	DEPTH_FUNCTION_LESS,
	DEPTH_FUNCTION_EQUAL,
	DEPTH_FUNCTION_LESS_OR_EQUAL,
	DEPTH_FUNCTION_GREATER,
	DEPTH_FUNCTION_NOT_EQUAL,
	DEPTH_FUNCTION_GREATER_OR_EQUAL,
	DEPTH_FUNCTION_ALWAYS
};

struct VulkanPipleineInfo
{
	PipelineType type;

	Topology topology;

	struct RasterizerInfo
	{
		CullMode cullMode;
		Face faceMode;
		PolygonMode polygonMode;
		float lineWidth;
	}rasInfo;

	struct BlendInfo
	{
		bool enableBlend;
		BlendFactor srcColorBlendFactor;
		BlendFactor dstColorBlendFactor;
		BlendOp colorBlendOp;
		BlendFactor srcAlphaBlendFactor;
		BlendFactor dstAlphaBlendFactor;
		BlendOp alphaBlendOp;
		float blendConstants[4];
	}blendInfo;

	struct DepthInfo
	{
		bool depthTest;
		bool depthWrite;
		DepthFunction depthFunction;
	}depthInfo;

	VulkanShader shaders[2];

	TinyImageFormat colorFormat;
	TinyImageFormat depthFormat;

	VulkanVertexBindingInfo bindingInfo;
	VulkanAttributeInfo attributeInfo[NUM_ATTRIBUTES];
	uint32_t numAttributes;

	VulkanDescriptorSetLayout* descriptorSetLayout;
};

struct VulkanPipeline
{
	PipelineType type;
	VkPipelineLayout pipelineLayout;
	VkRenderPass renderPass;
	VkPipeline pipeline;
};

void VulkanCreatePipeline(const Vulkan* const vulk, const VulkanPipleineInfo* const info, VulkanPipeline* pipeline);
void VulkanDestroyPipeline(const Vulkan* const vulk, VulkanPipeline* pipeline);

struct VulkanCommandPool
{
	VkCommandPool commandPool;
};

void VulkanCreateCommandPool(const Vulkan* const vulk, VulkanCommandPool* commandPool, const VulkanQueueType queueType);
void VulkanDestroyCommandPool(const Vulkan* const vulk, VulkanCommandPool* commandPool);

struct VulkanCommandBuffer
{
	VkCommandBuffer commandBuffer;
	bool isRendering;
};

void VulkanCreateCommandBuffer(const Vulkan* const vulk, const VulkanCommandPool* const commandPool,
	VulkanCommandBuffer* commandBuffer, const uint32_t numCommandBuffers);

void VulkanResetCommandBuffer(const VulkanCommandBuffer* const commandBuffer);

void VulkanBeginCommandBuffer(const VulkanCommandBuffer* const commandBuffer);


enum LoadOp
{
	LOAD_OP_LOAD,
	LOAD_OP_CLEAR,
	LOAD_OP_DONT_CARE
};

enum StoreOp
{
	STORE_OP_STORE,
	STORE_OP_DONT_CARE
};

struct VulkanBindRenderingAttachmentInfo
{
	VulkanRenderAttachment* colorAttachment;
	LoadOp colorAttachmentLoadOp;
	StoreOp colorAttachmentStoreOp;

	VulkanRenderAttachment* depthAttachment;
	LoadOp depthAttachmentLoadOp;
	StoreOp depthAttachmentStoreOp;
};

void VulkanBindRenderAttachment(VulkanCommandBuffer* commandBuffer, const VulkanBindRenderingAttachmentInfo* const info);

void VulkanBindPipeline(const VulkanCommandBuffer* commandBuffer, const VulkanPipeline* const pipeline);

struct ViewportInfo
{
	float x;
	float y;
	float width;
	float height;
	float minDepth;
	float maxDepth;
};

struct ScissorInfo
{
	int32_t x;
	int32_t y;
	uint32_t width;
	uint32_t height;
};

void VulkanSetViewport(const VulkanCommandBuffer* const commandBuffer, const ViewportInfo* const viewportInfo);
void VulkanSetScissor(const VulkanCommandBuffer* const commandBuffer, const ScissorInfo* const scissorInfo);

void VulkanDraw(const VulkanCommandBuffer* const commandBuffer, const uint32_t vertexCount,
	const uint32_t instanceCount, const uint32_t firstVertex, const uint32_t firstInstance);

void VulkanDrawIndexed(const VulkanCommandBuffer* const commandBuffer, const uint32_t indexCount,
	const uint32_t instanceCount, const uint32_t firstIndex, const uint32_t vertexOffset, const uint32_t firstInstance);

void VulkanDispatch(const VulkanCommandBuffer* const commandBuffer, const uint32_t x, const uint32_t y, const uint32_t z);

void VulkanEndCommandBuffer(const VulkanCommandBuffer* const commandBuffer);

struct VulkanFence
{
	VkFence fence;
};

void VulkanCreateFence(const Vulkan* const vulk, VulkanFence* fence);

void VulkanDestroyFence(const Vulkan* const vulk, VulkanFence* fence);

void VulkanWaitForFence(const Vulkan* const vulk, const VulkanFence* const fence);

struct VulkanSemaphore
{
	VkSemaphore semaphore;
};

void VulkanCreateSemaphore(const Vulkan* const vulk, VulkanSemaphore* semaphore);

void VulkanDestroySemaphore(const Vulkan* const vulk, VulkanSemaphore* semaphore);

void VulkanAcquireNextImage(const Vulkan* const vulk, const VulkanSwapChain* const swapChain,
	const VulkanSemaphore* const imageAvailableSemaphore, uint32_t* imageIndex);


struct VulkanQueueSubmitInfo
{
	VulkanQueue* queue;
	uint32_t numWaitSemaphores;
	VulkanSemaphore* waitSemaphores;	//wait for image to become available
	uint32_t numSignalSemaphores;
	VulkanSemaphore* signalSemaphores; //signal for when the command buffer is finished executing
	VulkanFence* fence;
	VulkanCommandBuffer* commandBuffer;
};

void VulkanQueueSubmit(Vulkan* vulk, VulkanQueueSubmitInfo* info);

struct VulkanPresentInfo
{
	VulkanQueue* queue;
	VulkanSemaphore* waitSemaphore;	//wait for the command buffer to finish execution
	VulkanSwapChain* swapChain;
	uint32_t imageIndex;
};

void VulkanQueuePresent(Vulkan* vulk, VulkanPresentInfo* info);

void VulkanWaitDeviceIdle(Vulkan* vulk);

void VulkanWaitQueueIdle(VulkanQueue* queue);

void VulkanOnResize(Vulkan* vulk, VulkanSwapChain* swapChain, VulkanRenderAttachment* depthBuffer);

enum VulkanBufferType
{
	BUFFER_TYPE_VERTEX_BUFFER = 0x00000001,
	BUFFER_TYPE_INDEX_BUFFER = 0x00000002,
	BUFFER_TYPE_UNIFORM_BUFFER = 0x00000004,
	BUFFER_TYPE_STORAGE_BUFFER = 0x00000008
};

enum VulkanAccess
{
	ACCESS_GPU,
	ACCESS_CPU_GPU
};

enum VulkanIndexType
{
	INDEX_TYPE_UINT16,
	INDEX_TYPE_UINT32
};

struct VulkanBufferInfo
{
	uint32_t size;
	uint32_t type;
	VulkanAccess access;

	VulkanIndexType indexType;

	//Will only copy data when creating the buffer if access = SE_GPU
	void* data;
};

struct VulkanBuffer
{
	VkBuffer buffer;
	VmaAllocation allocation;
	uint32_t type;
	VulkanIndexType indexType;
};

void VulkanCreateBuffer(const Vulkan* const vulk, const VulkanBufferInfo* const bufferInfo, VulkanBuffer* buffer);

void VulkanDestroyBuffer(const Vulkan* const vulk, VulkanBuffer* buffer);

void VulkanMapMemory(const Vulkan* const vulk, const VulkanBuffer* const buffer, void** data);

void VulkanUnmapMemory(const Vulkan* const vulk, const VulkanBuffer* const buffer);

void VulkanBindBuffer(const VulkanCommandBuffer* const commandBuffer, const uint32_t bindingLocation, 
	const uint32_t offset, const VulkanBuffer* const buffer);

enum VulkanImageLayout
{
	LAYOUT_UNDEFINED,
	LAYOUT_GENERAL,
	LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
	LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
	LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
	LAYOUT_SHADER_READ_ONLY_OPTIMAL,
	LAYOUT_TRANSFER_SRC_OPTIMAL,
	LAYOUT_TRANSFER_DST_OPTIMAL,
	LAYOUT_PREINITIALIZED,
	LAYOUT_PRESENT_SRC = 1000001002
};

void VulkanTransitionImage(const VulkanCommandBuffer* const commandBuffer, const VulkanRenderAttachment* const attachment,
	const VulkanImageLayout oldLayout, const VulkanImageLayout newLayout);

struct VulkanTexture
{
	VkImage image;
	VmaAllocation allocation;
	VkImageView imageView;
};

void VulkanCreateTexture(const Vulkan* const vulk, const char* filename, VulkanTexture* texture);
void VulkanDestroyTexture(const Vulkan* const vulk, VulkanTexture* texture);

enum Filter
{
	NEAREST_FILTER,
	LINEAR_FILTER
};

enum AddressMode
{
	ADDRESS_MODE_REPEAT,
	ADDRESS_MODE_MIRRORED_REPEAT,
	ADDRESS_MODE_CLAMP_TO_EDGE,
	ADDRESS_MODE_CLAMP_TO_BORDER
};

enum MipMapMode
{
	MIPMAP_MODE_NEAREST,
	MIPMAP_MODE_LINEAR
};

struct VulkanSamplerInfo
{
	Filter magFilter;
	Filter minFilter;
	AddressMode u;
	AddressMode v;
	AddressMode w;
	MipMapMode mipMapMode;
	float maxAnisotropy;
	float mipLoadBias;
	float minLod;
	float maxLod;
};

struct VulkanSampler
{
	VkSampler sampler;
};

void VulkanCreateSampler(const Vulkan* const vulk, const VulkanSamplerInfo* const info, VulkanSampler* sampler);
void VulkanDestroySampler(const Vulkan* const vulk, VulkanSampler* sampler);

struct VulkanDescriptorSetInfo
{
	VulkanDescriptorSetLayout* layouts;
	uint32_t numDescriptorSets;
};

struct VulkanUpdateDescriptorSetInfo
{
	uint32_t index;
	uint32_t binding;

	struct Buffer
	{
		VulkanBuffer* buffer;
		uint32_t offset;
		uint32_t size;
		VulkanDescriptorType type;
	}bufferInfo;

	struct Image
	{
		VulkanTexture* texture;
		VulkanSampler* sampler;
	}imageInfo;
};

struct VulkanDescriptorSet
{
	VkDescriptorSet* descriptorSets;
};

void VulkanCreateDescriptorSet(const Vulkan* const vulk, const VulkanDescriptorSetInfo* const info, VulkanDescriptorSet* ds);
void VulkanDestroyDescriptorSet(VulkanDescriptorSet* ds);
void VulkanUpdateDescriptorSet(const Vulkan* const vulk, const VulkanUpdateDescriptorSetInfo* const info, 
	const VulkanDescriptorSet* const  ds);
void VulkanBindDescriptorSet(const VulkanCommandBuffer* const commandBuffer, 
	const VulkanPipeline* const pipeline, const uint32_t index, const VulkanDescriptorSet* const ds);