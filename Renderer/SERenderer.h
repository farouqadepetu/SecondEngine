#pragma once

//stb
#include "../ThirdParty/stb_ds.h"

//VULKAN
#define VK_USE_PLATFORM_WIN32_KHR
#define IMGUI_IMPL_VULKAN_NO_PROTOTYPES
#include "../ThirdParty/imgui/imgui_impl_vulkan.h"
#include "Vulkan/VulkanSDK/Include/volk/volk.h"
#include <vulkan/vk_enum_string_helper.h>
#include <vma/vk_mem_alloc.h>

//DIRECTX
#include "DirectX/AgilitySDK/include/d3d12.h"
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include "DirectX/DMA/D3D12MemAlloc.h"
#include "../ThirdParty/imgui/imgui_impl_dx12.h"

#pragma comment(lib, "dxguid")

//SE
#include "SEDDSLoader.h"
#include "SEWindow.h"
#include "../FileSystem/SEFileSystem.h"

//THRID PARTY
#include "../ThirdParty/TinyImage/tinyimageformat_apis.h"

#if defined(CreateSemaphore)
#undef CreateSemaphore
#endif

struct Fence
{
	struct
	{
		VkFence fence;
	}vk;

	struct
	{
		ID3D12Fence* fence;
		HANDLE fenceEvent;
		uint64_t fenceValue;
	}dx;
};

struct Semaphore
{
	struct
	{
		VkSemaphore semaphore;
	}vk;

	struct
	{

	}dx;
};

enum QueueType
{
	QUEUE_TYPE_GRAPHICS,
	QUEUE_TYPE_COMPUTE,
	QUEUE_TYPE_COPY
};

struct Queue
{
	struct
	{
		VkQueue queue;
	}vk;

	struct
	{
		ID3D12CommandQueue* queue;
		Fence fence;
	}dx;
};

struct CommandBuffer
{
	struct
	{
		VkCommandPool commandPool;
		VkCommandBuffer commandBuffer;
	}vk;

	struct
	{
		ID3D12CommandAllocator* commandAllocator;
		ID3D12GraphicsCommandList* commandList;

	}dx;

	Fence fence;
	Semaphore semaphore;

	QueueType type;
	bool isRendering;
};

struct Renderer
{
	struct
	{
		VkInstance instance;
		VkPhysicalDevice physicalDevice;
		uint32_t familyIndices[3];
		VkDevice logicalDevice;
		VmaAllocator allocator;
#ifdef _DEBUG
		const bool enableValidationLayers = true;
#else
		const bool enableValidationLayers = false;
#endif
		VkDebugUtilsMessengerEXT debugMessenger;
	}vk;

	struct
	{
		IDXGIFactory7* factory;
		IDXGIAdapter4* adapter;
		D3D_FEATURE_LEVEL featureLevel;
		ID3D12Device* device;
		D3D12MA::Allocator* allocator;
#if defined(_DEBUG)
		ID3D12Debug4* debug;
		ID3D12InfoQueue* infoQueue;
#endif
	}dx;

	//Used for transitioning resources to their initial state
	Queue queue;
	CommandBuffer commandBuffer;
};

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

enum ResourceState
{
	RESOURCE_STATE_UNDEFINED = 0,
	RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER = 0x1,
	RESOURCE_STATE_INDEX_BUFFER = 0x2,
	RESOURCE_STATE_RENDER_TARGET = 0x4,
	RESOURCE_STATE_UNORDERED_ACCESS = 0x8,
	RESOURCE_STATE_DEPTH_WRITE = 0x10,
	RESOURCE_STATE_DEPTH_READ = 0x20,
	RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE = 0x40,
	RESOURCE_STATE_PIXEL_SHADER_RESOURCE = 0x80,
	RESOURCE_STATE_ALL_SHADER_RESOURCE = (0x40 | 0x80),
	RESOURCE_STATE_STREAM_OUT = 0x100,
	RESOURCE_STATE_INDIRECT_ARGUMENT = 0x200,
	RESOURCE_STATE_COPY_DEST = 0x400,
	RESOURCE_STATE_COPY_SOURCE = 0x800,
	RESOURCE_STATE_GENERIC_READ = (((((0x1 | 0x2) | 0x40) | 0x80) | 0x200) | 0x800),
	RESOURCE_STATE_PRESENT = 0x1000,
	RESOURCE_STATE_COMMON = 0x2000
};

struct RenderTargetInfo
{
	uint32_t width;
	uint32_t height;
	TinyImageFormat format;
	ClearValue clearValue;
	ResourceState initialState;
};

struct RenderTarget
{
	struct
	{
		VkImage image;
		VkImageView imageView;
		VmaAllocation allocation;
	}vk;

	struct
	{
		ID3D12Resource* resource;
		D3D12MA::Allocation* allocation;
		uint32_t descriptorId;
	}dx;

	RenderTargetInfo info;
};

struct SwapChainInfo
{
	Window* window;
	Queue* queue;
	uint32_t width;
	uint32_t height;
	TinyImageFormat format;
	ClearValue clearValue;
};

struct SwapChain
{
	struct
	{
		VkSurfaceKHR surface;
		VkSwapchainKHR swapChain;
	}vk;

	struct
	{
		IDXGISwapChain3* swapChain;
		uint32_t flags;
	}dx;

	uint32_t numRenderTargets;
	RenderTarget* pRenderTargets;
	SwapChainInfo info;
};

enum ShaderType
{
	SHADER_TYPE_VERTEX,
	SHADER_TYPE_PIXEL,
	SHADER_TYPE_COMPUTE
};

struct ShaderInfo
{
	const char* glslFilename;
	const char* hlslFilename;
	ShaderType type;
};

struct Shader
{
	struct
	{
		VkShaderModule shaderModule;
		VkPipelineShaderStageCreateInfo shaderCreateInfo;
	}vk;

	struct
	{
		D3D12_SHADER_BYTECODE shader;
	}dx;
};

enum DescriptorType
{
	DESCRIPTOR_TYPE_SAMPLER,
	DESCRIPTOR_TYPE_TEXTURE,
	DESCRIPTOR_TYPE_RW_TEXTURE,
	DESCRIPTOR_TYPE_BUFFER,
	DESCRIPTOR_TYPE_RW_BUFFER,
	DESCRIPTOR_TYPE_UNIFORM_BUFFER
};

enum Stage
{
	STAGE_VERTEX = 0x01,
	STAGE_PIXEL = 0x02,
	STAGE_COMPUTE = 0x04
};

enum UpdateFrequency
{
	UPDATE_FREQUENCY_PER_NONE,
	UPDATE_FREQUENCY_PER_FRAME,
	UPDATE_FREQUENCY_PER_SAMPLER,
	UPDATE_FREQUENCY_COUNT
};

enum PipelineType
{
	PIPELINE_TYPE_GRAPHICS,
	PIPELINE_TYPE_COMPUTE
};

#define MAX_NUM_ROOT_PARAMETER_INFOS 8
struct RootSignatureInfo
{
	uint32_t numRootParameterInfos;
	struct RootParameterInfo
	{
		DescriptorType type;
		UpdateFrequency updateFrequency;
		uint32_t binding;
		uint32_t baseRegister;
		uint32_t registerSpace;
		uint32_t numDescriptors;
		uint32_t stages;
	}rootParameterInfos[MAX_NUM_ROOT_PARAMETER_INFOS];

	bool useInputLayout;
};

struct RootSignature
{
	struct
	{
		VkPipelineLayout pipelineLayout;
		VkDescriptorSetLayout descriptorSetLayouts[UPDATE_FREQUENCY_COUNT];
	}vk;

	struct
	{
		ID3D12RootSignature* rootSignature;
		int32_t rootParamterIndices[UPDATE_FREQUENCY_COUNT];
	}dx;

	PipelineType pipelineType;
	bool useInputLayout;
};

enum UpdateType
{
	UPDATE_TYPE_SAMPLER,
	UPDATE_TYPE_BUFFER,
	UPDATE_TYPE_RW_BUFFER,
	UPDATE_TYPE_UNIFORM_BUFFER,
	UPDATE_TYPE_TEXTURE,
	UPDATE_TYPE_RW_TEXTURE
};

enum InputRate
{
	INPUT_RATE_PER_VERTEX,
	INPUT_RATE_PER_INSTANCE
};

struct VertexBinding
{
	uint32_t binding;
	uint32_t stride;
	InputRate inputRate;
};

struct VertexAttributes
{
	const char* semanticName;
	uint32_t semanticIndex;
	uint32_t binding;
	uint32_t location;
	TinyImageFormat format;
	uint32_t offset;
};

#define MAX_NUM_VERTEX_ATTRIBUTES 8
struct VertexInputInfo
{
	VertexBinding vertexBinding;

	uint32_t numVertexAttributes;
	VertexAttributes vertexAttributes[MAX_NUM_VERTEX_ATTRIBUTES];
};

enum FillMode
{
	FILL_MODE_SOLID,
	FILL_MODE_WIREFRAME
};

enum Topology
{
	TOPOLOGY_POINT_LIST,
	TOPOLOGY_LINE_LIST,
	TOPOLOGY_TRIANGLE_LIST,
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

enum Filter
{
	FILTER_NEAREST,
	FILTER_LINEAR
};

enum AddressMode
{
	ADDRESS_MODE_REPEAT,
	ADDRESS_MODE_MIRRORED,
	ADDRESS_MODE_CLAMP_TO_EDGE,
	ADDRESS_MODE_CLAMP_TO_BORDER
};

enum MipMapMode
{
	MIPMAP_MODE_NEAREST,
	MIPMAP_MODE_LINEAR
};

struct PipelineInfo
{
	PipelineType type;
	Shader* pVertexShader;
	Shader* pPixelShader;
	Shader* pComputeShader;
	RootSignature* pRootSignature;
	VertexInputInfo* pVertexInputInfo;

	struct RasterizerInfo
	{
		CullMode cullMode;
		Face faceMode;
		FillMode fillMode;
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
	}blendInfo;

	struct DepthInfo
	{
		bool depthTestEnable;
		bool depthWriteEnable;
		DepthFunction depthFunction;
	}depthInfo;

	Topology topology;
	TinyImageFormat renderTargetFormat;
	TinyImageFormat depthFormat;
};

struct Pipeline
{
	struct
	{
		VkPipeline pipeline;
	}vk;

	struct
	{
		ID3D12PipelineState* pipeline;
	}dx;

	PipelineType type;
	Topology topology;
	RootSignature* pRootSignature;
};

enum IndexType
{
	INDEX_TYPE_UINT16,
	INDEX_TYPE_UINT32
};

enum BufferType
{
	BUFFER_TYPE_VERTEX = 0x1,
	BUFFER_TYPE_INDEX = 0x2,
	BUFFER_TYPE_UNIFORM = 0x4,
	BUFFER_TYPE_BUFFER = 0x8,
	BUFFER_TYPE_RW_BUFFER = 0x10
};

enum MemoryUsage
{
	MEMORY_USAGE_GPU_ONLY,
	MEMORY_USAGE_CPU_ONLY,
	MEMORY_USAGE_CPU_TO_GPU,
	MEMORY_USAGE_GPU_TO_CPU
};

struct BufferInfo
{
	uint32_t type;
	MemoryUsage usage;
	uint32_t size;
	ResourceState initialState;

	//This data is only needed if type has BUFFER_TYPE_RW_BUFFER or BUFFER_TYPE_BUFFER
	uint32_t firstElement;
	uint32_t numElements;
	uint32_t stride;

	//Will only copy data when creating the buffer if usage = MEMORY_USAGE_GPU_ONLY and data != nullptr
	void* data;
};

struct Buffer
{
	struct
	{
		VkBuffer buffer;
		VmaAllocation allocation;
	}vk;

	struct
	{
		D3D12MA::Allocation* allocation;
		ID3D12Resource* resource;

		uint32_t cpuCbDescriptorId;
		uint32_t cpuSrvDescriptorId;
		uint32_t cpuUavDescriptorId;

		int32_t gpuCbDescriptorId;
		uint32_t gpuSrvDescriptorId;
		uint32_t gpuUavDescriptorId;
	}dx;

	uint32_t size;
	uint32_t type;
};

enum TextureType
{
	TEXTURE_TYPE_TEXTURE = 0x1,
	TEXTURE_TYPE_RW_TEXTURE = 0x2
};

struct TextureInfo
{
	const char* filename;

	//ONLY NEED TO FILL THIS OUT IF FILENAME IS NULLPTR (YOU WANT TO CREATE AN EMPTY TEXTURE)
	uint32_t width;
	uint32_t height;
	uint32_t depth;
	uint32_t arraySize;
	uint32_t mipCount;
	TinyImageFormat format;
	TextureDimension dimension;
	TextureType type;
	bool isCubeMap;
	ResourceState initialState;
};

struct Texture
{
	struct
	{
		VkImage image;
		VmaAllocation allocation;
		VkImageView imageView;
	}vk;

	struct
	{
		ID3D12Resource* resource;
		D3D12MA::Allocation* allocation;

		uint32_t cpuSrvDescriptorId;
		uint32_t cpuUavDescriptorId;

		uint32_t gpuSrvDescriptorId;
		uint32_t gpuUavDescriptorId;
	}dx;

	TextureType type;
};

struct SamplerInfo
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

struct Sampler
{
	struct
	{
		VkSampler sampler;
	}vk;

	struct
	{
		uint32_t cpuDescriptorId;
		uint32_t gpuDescriptorId;
	}dx;
};

struct DescriptorSetInfo
{
	UpdateFrequency updateFrequency;
	const RootSignature* pRootSignature;
	uint32_t numSets;
};

struct DescriptorSet
{
	struct
	{
		VkDescriptorSet* pDescriptorSets;
	}vk;

	struct
	{
		int32_t* heapIndices;
	}dx;

	UpdateFrequency updateFrequency;
	const RootSignature* pRootSignature;
};

struct UpdateDescriptorSetInfo
{
	UpdateType type;
	uint32_t binding;

	union
	{
		Buffer* pBuffer;
		Texture* pTexture;
		Sampler* pSampler;
	};
};

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

#define MAX_NUM_BARRIERS 32
enum BarrierType
{
	BARRIER_TYPE_BUFFER,
	BARRIER_TYPE_TEXTURE,
	BARRIER_TYPE_RENDER_TARGET
};

struct BarrierInfo
{
	BarrierType type;

	union
	{
		const Buffer* pBuffer;
		const Texture* pTexture;
		const RenderTarget* pRenderTarget;
	};

	ResourceState currentState;
	ResourceState newState;
};

struct BindRenderTargetInfo
{
	RenderTarget* pRenderTarget;
	LoadOp renderTargetLoadOp;
	StoreOp renderTargetStoreOp;

	RenderTarget* pDepthTarget;
	LoadOp depthTargetLoadOp;
	StoreOp depthTargetStoreOp;
};

struct QueueSubmitInfo
{
	const Queue* pQueue;
	uint32_t numWaitSemaphores;
	Semaphore* waitSemaphores;
	uint32_t numSignalSemaphores;
	Semaphore* signalSemaphores;
	const Fence* pFence;
	const CommandBuffer* pCommandBuffer;
};

struct PresentInfo
{
	Queue* pQueue;
	uint32_t numWaitSemaphores;
	Semaphore* waitSemaphores;	//wait for the command buffer to finish execution
	SwapChain* pSwapChain;
	uint32_t imageIndex;
};

enum RendererAPI
{
	DIRECTX,
	VULKAN
};

struct UiInfo
{
	Window* pWindow;
	Queue* pQueue;
	uint32_t numImages;
	uint32_t numFrames;
	TinyImageFormat renderTargetFormat;
	TinyImageFormat depthFormat;
};

//CALLABLE
extern void (*InitRenderer)(Renderer* pRenderer, const char*);
extern void (*DestroyRenderer)(Renderer* pRenderer);

extern void (*CreateFence)(const Renderer* const pRenderer, Fence* pFence);
extern void (*DestroyFence)(const Renderer* const pRenderer, Fence* pFence);
extern void (*WaitForFence)(const Renderer* const pRenderer, Fence* pFence);

extern void (*CreateQueue)(const Renderer* const pRenderer, const QueueType type, Queue* pQueue);
extern void (*DestroyQueue)(const Renderer* const pRenderer, Queue* pQueue);
extern void (*WaitQueueIdle)(const Renderer* const pRenderer, Queue* pQueue);

extern void (*CreateRenderTarget)(const Renderer* const pRenderer, const RenderTargetInfo* const pInfo, RenderTarget* pRenderTarget);
extern void (*DestroyRenderTarget)(const Renderer* const pRenderer, RenderTarget* pRenderTarget);
extern void (*BindRenderTarget)(CommandBuffer* pCommandBuffer, const BindRenderTargetInfo* const pInfo);

extern void (*CreateSwapChain)(const Renderer* const pRenderer, const SwapChainInfo* const pInfo, SwapChain* pSwapChain);
extern void (*DestroySwapChain)(const Renderer* const pRenderer, SwapChain* pSwapChain);

extern void (*CreateShader)(const Renderer* const pRenderer, const ShaderInfo* const pInfo, Shader* pShader);
extern void (*DestroyShader)(const Renderer* const pRenderer, Shader* pShader);

extern void (*CreateRootSignature)(const Renderer* const pRenderer, const RootSignatureInfo* const pInfo, RootSignature* pRootSignature);
extern void (*DestroyRootSignature)(const Renderer* const pRenderer, RootSignature* pRootSignature);

extern void (*CreatePipeline)(const Renderer* const pRenderer, const PipelineInfo* const pInfo, Pipeline* pPipeline);
extern void (*DestroyPipeline)(const Renderer* const pRenderer, Pipeline* pPipeline);
extern void (*BindPipeline)(const CommandBuffer* pCommandBuffer, const Pipeline* const pPipeline);

extern void (*CreateSemaphore)(const Renderer* const pRenderer, Semaphore* semaphore);
extern void (*DestroySemaphore)(const Renderer* const pRenderer, Semaphore* semaphore);

extern void (*CreateCommandBuffer)(const Renderer* const pRenderer, const QueueType type, CommandBuffer* pCommandBuffer);
extern void (*DestroyCommandBuffer)(const Renderer* const pRenderer, CommandBuffer* pCommandBuffer);
extern void (*ResetCommandBuffer)(const Renderer* const pRenderer, const CommandBuffer* const pCommandBuffer);
extern void (*BeginCommandBuffer)(const CommandBuffer* const pCommandBuffer);
extern void (*EndCommandBuffer)(const CommandBuffer* const pCommandBuffer);

extern void (*CreateBuffer)(const Renderer* const pRenderer, const BufferInfo* pInfo, Buffer* pBuffer);
extern void (*DestroyBuffer)(const Renderer* const pRenderer, Buffer* pBuffer);
extern void (*MapMemory)(const Renderer* const pRenderer, const Buffer* const pBuffer, void** ppData);
extern void (*UnmapMemory)(const Renderer* const pRenderer, const Buffer* const pBuffer);
extern void (*BindVertexBuffer)(const CommandBuffer* const pCommandBuffer, const uint32_t stride, const uint32_t bindingLocation,
	const uint32_t offset, const Buffer* const pVertexBuffer);
extern void (*BindIndexBuffer)(const CommandBuffer* const pCommandBuffer, const uint32_t offset,
	const IndexType indexType, const Buffer* const pIndexBuffer);

extern void (*CreateTexture)(const Renderer* const pRenderer, const TextureInfo* const pInfo, Texture* pTexture);
extern void (*DestroyTexture)(const Renderer* const pRenderer, Texture* pTexture);

extern void (*CreateSampler)(const Renderer* const pRenderer, const SamplerInfo* const pInfo, Sampler* pSampler);
extern void (*DestroySampler)(const Renderer* const pRenderer, Sampler* pSampler);

extern void (*CreateDescriptorSet)(const Renderer* const pRenderer, const DescriptorSetInfo* const pInfo, DescriptorSet* pDescriptorSet);
extern void (*DestroyDescriptorSet)(DescriptorSet* pDescriptorSet);
extern void (*UpdateDescriptorSet)(const Renderer* const pRenderer, const DescriptorSet* const  pDescriptorSet,
	const uint32_t index, const uint32_t numInfos, const UpdateDescriptorSetInfo* const pInfos);
extern void (*BindDescriptorSet)(const CommandBuffer* const pCommandBuffer, const uint32_t index,
	const uint32_t firstSet, const DescriptorSet* const pDescriptorSet);

extern void (*AcquireNextImage)(const Renderer* const pRenderer, const SwapChain* const pSwapChain,
	const Semaphore* const pSemaphore, uint32_t* pImageIndex);

extern void (*SetViewport)(const CommandBuffer* const pCommandBuffer, const ViewportInfo* const pInfo);
extern void (*SetScissor)(const CommandBuffer* const pCommandBuffer, const ScissorInfo* const pInfo);
extern void (*Draw)(const CommandBuffer* const pCommandBuffer, const uint32_t vertexCount,
	const uint32_t instanceCount, const uint32_t firstVertex, const uint32_t firstInstance);
extern void (*DrawIndexed)(const CommandBuffer* const pCommandBuffer, const uint32_t indexCount,
	const uint32_t instanceCount, const uint32_t firstIndex, const uint32_t vertexOffset, const uint32_t firstInstance);
extern void (*Dispatch)(const CommandBuffer* const pCommandBuffer, const uint32_t x, const uint32_t y, const uint32_t z);

extern void (*QueueSubmit)(const QueueSubmitInfo* const pInfo);
extern void (*QueuePresent)(const PresentInfo* const pInfo);

extern void (*ResourceBarrier)(const CommandBuffer* const pCommandBuffer, 
	const uint32_t numBarrierInfos, const BarrierInfo* const pBarrierInfos);

extern void (*InitUI)(const Renderer* const pRenderer, const UiInfo* const pInfo);
extern void (*DestroyUI)(const Renderer* const pRenderer);
extern void (*RenderUI)(const CommandBuffer* const pCommandBuffer);

void InitSE();
void ExitSE();
void OnRendererApiSwitch();
const char* GetRendererApi();
void NewFrameApi();