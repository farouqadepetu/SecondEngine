#define STB_DS_IMPLEMENTATION
#include "SERenderer.h"

RendererAPI gRendererAPI = DIRECTX;

//DIRECTX
extern void DirectXInitRenderer(Renderer* pRenderer, const char* appName);
extern void DirectXDestroyRenderer(Renderer* pRenderer);

extern void DirectXCreateFence(const Renderer* const pRenderer, Fence* pFence);
extern void DirectXDestroyFence(const Renderer* const pRenderer, Fence* pFence);
extern void DirectXWaitForFence(const Renderer* const pRenderer, Fence* pFence);

extern void DirectXCreateSemaphore(const Renderer* const pRenderer, Semaphore* pSemaphore);
extern void DirectXDestroySemaphore(const Renderer* const pRenderer, Semaphore* pSemaphore);

extern void DirectXCreateQueue(const Renderer* const pRenderer, const QueueType type, Queue* pQueue);
extern void DirectXDestroyQueue(const Renderer* const pRenderer, Queue* pQueue);
extern void DirectXWaitQueueIdle(const Renderer* const pRenderer, Queue* pQueue);

extern void DirectXCreateRenderTarget(const Renderer* const pRenderer, const RenderTargetInfo* const pInfo, RenderTarget* pRenderTarget);
extern void DirectXDestroyRenderTarget(const Renderer* const pRenderer, RenderTarget* pRenderTarget);

extern void DirectXCreateSwapChain(const Renderer* const pRenderer, const SwapChainInfo* const pInfo, SwapChain* pSwapChain);
extern void DirectXDestroySwapChain(const Renderer* const pRenderer, SwapChain* pSwapChain);

extern void DirectXCreateShader(const Renderer* const pRenderer, const ShaderInfo* const pInfo, Shader* pShader);
extern void DirectXDestroyShader(const Renderer* const pRenderer, Shader* pShader);

extern void DirectXCreateRootSignature(const Renderer* const pRenderer, const RootSignatureInfo* const pInfo, RootSignature* pRootSignature);
extern void DirectXDestroyRootSignature(const Renderer* const pRenderer, RootSignature* pRootSignature);

extern void DirectXCreatePipeline(const Renderer* const pRenderer, const PipelineInfo* const pInfo, Pipeline* pPipeline);
extern void DirectXDestroyPipeline(const Renderer* const pRenderer, Pipeline* pPipeline);

extern void DirectXCreateCommandBuffer(const Renderer* const pRenderer, const QueueType type, CommandBuffer* pCommandBuffer);
extern void DirectXDestroyCommandBuffer(const Renderer* const pRenderer, CommandBuffer* pCommandBuffer);
extern void DirectXResetCommandBuffer(const Renderer* const pRenderer, const CommandBuffer* const pCommandBuffer);
extern void DirectXBeginCommandBuffer(const CommandBuffer* const pCommandBuffer);
extern void DirectXEndCommandBuffer(const CommandBuffer* const pCommandBuffer);

extern void DirectXAcquireNextImage(const Renderer* const pRenderer, const SwapChain* const pSwapChain,
	const Semaphore* const pSemaphore, uint32_t* pImageIndex);

extern void DirectXBindRenderTarget(CommandBuffer* pCommandBuffer, const BindRenderTargetInfo* const pInfo);

extern void DirectXSetViewport(const CommandBuffer* const pCommandBuffer, const ViewportInfo* const viewportInfo);
extern void DirectXSetScissor(const CommandBuffer* const pCommandBuffer, const ScissorInfo* const scissorInfo);

extern void DirectXBindPipeline(CommandBuffer* pCommandBuffer, const Pipeline* const pPipeline);

extern void DirectXDraw(const CommandBuffer* const pCommandBuffer, const uint32_t vertexCount,
	const uint32_t instanceCount, const uint32_t firstVertex, const uint32_t firstInstance);

extern void DirectXDrawIndexed(const CommandBuffer* const pCommandBuffer, const uint32_t indexCount,
	const uint32_t instanceCount, const uint32_t firstIndex, const uint32_t vertexOffset, const uint32_t firstInstance);

extern void DirectXDispatch(const CommandBuffer* const pCommandBuffer, const uint32_t x, const uint32_t y, const uint32_t z);

extern void DirectXQueueSubmit(const QueueSubmitInfo* const pInfo);

extern void DirectXQueuePresent(const PresentInfo* const pInfo);

extern void DirectXCreateBuffer(const Renderer* const pRenderer, const BufferInfo* pInfo, Buffer* pBuffer);
extern void DirectXDestroyBuffer(const Renderer* const pRenderer, Buffer* pBuffer);

extern void DirectXMapMemory(const Renderer* const pRenderer, const Buffer* const pBuffer, void** ppData);
extern void DirectXUnmapMemory(const Renderer* const pRenderer, const Buffer* const pBuffer);

extern void DirectXBindVertexBuffer(const CommandBuffer* const pCommandBuffer, const uint32_t stride, const uint32_t bindingLocation,
	const uint32_t offset, const Buffer* const pVertexBuffer);

extern void DirectXBindIndexBuffer(const CommandBuffer* const pCommandBuffer, const uint32_t offset,
	const IndexType indexType, const Buffer* const pIndexBuffer);

extern void DirectXCreateTexture(const Renderer* const pRenderer, const TextureInfo* const pInfo, Texture* pTexture);
extern void DirectXDestroyTexture(const Renderer* const pRenderer, Texture* pTexture);

extern void DirectXCreateSampler(const Renderer* const pRenderer, const SamplerInfo* const pInfo, Sampler* pSampler);
extern void DirectXDestroySampler(const Renderer* const pRenderer, Sampler* pSampler);

extern void DirectXCreateDescriptorSet(const Renderer* const pRenderer, const DescriptorSetInfo* const pInfo, DescriptorSet* pDescriptorSet);
extern void DirectXDestroyDescriptorSet(DescriptorSet* pDescriptorSet);
extern void DirectXUpdateDescriptorSet(const Renderer* const pRenderer, const DescriptorSet* const pDescriptorSet, uint32_t index,
	const uint32_t numInfos, const UpdateDescriptorSetInfo* const pInfos);

extern void DirectXBindDescriptorSet(const CommandBuffer* const pCommandBuffer, const uint32_t index, const uint32_t firstSet,
	const DescriptorSet* const pDescriptorSet);

extern void DirectXBindRootConstants(const CommandBuffer* const pCommandBuffer, uint32_t numValues, uint32_t stride, const void* pData, uint32_t offset);

extern void DirectXResourceBarrier(const CommandBuffer* const pCommandBuffer, const uint32_t numBarrierInfos, const BarrierInfo* const pBarrierInfos);

extern void DirectXInitUI(const Renderer* const pRenderer, const UIDesc* const pInfo);
extern void DirectXDestroyUI(const Renderer* const pRenderer);
extern void DirectXRenderUI(const CommandBuffer* const pCommandBuffer);

extern void DirectXSwapChainResize(const Renderer* const pRenderer, const SwapChainInfo* const pInfo, SwapChain* pSwapChain);

//VULKAN
extern void VulkanInitRenderer(Renderer* pRenderer, const char* appName);
extern void VulkanDestroyRenderer(Renderer* pRenderer);

extern void VulkanCreateQueue(const Renderer* const pRenderer, const QueueType queueType, Queue* pQueue);
extern void VulkanDestroyQueue(const Renderer* const pRenderer, Queue* pQueue);
extern void VulkanWaitQueueIdle(const Renderer* const pRenderer, Queue* pQueue);

extern void VulkanCreateRenderTarget(const Renderer* const pRenderer,
	const RenderTargetInfo* const pInfo, RenderTarget* pRenderTarget);

extern void VulkanDestroyRenderTarget(const Renderer* const pRenderer, RenderTarget* pRenderTarget);

extern void VulkanCreateSwapChain(const Renderer* const pRenderer, const SwapChainInfo* const pInfo, SwapChain* pSwapChain);
extern void VulkanDestroySwapChain(const Renderer* const pRenderer, SwapChain* pSwapChain);

extern void VulkanCreateShader(const Renderer* const pRenderer, const ShaderInfo* const pInfo, Shader* pShader);
extern void VulkanDestroyShader(const Renderer* const pRenderer, Shader* pShader);

extern void VulkanCreateRootSignature(const Renderer* const pRenderer, const RootSignatureInfo* const pInfo, RootSignature* pRootSignature);
extern void VulkanDestroyRootSignature(const Renderer* const pRenderer, RootSignature* pRootSignature);

extern void VulkanCreatePipeline(const Renderer* const pRenderer, const PipelineInfo* const pInfo, Pipeline* pPipeline);
extern void VulkanDestroyPipeline(const Renderer* const pRenderer, Pipeline* pPipeline);

extern void VulkanCreateCommandBuffer(const Renderer* const pRenderer, const QueueType type, CommandBuffer* pCommandBuffer);
extern void VulkanDestroyCommandBuffer(const Renderer* const pRenderer, CommandBuffer* pCommandBuffer);
extern void VulkanResetCommandBuffer(const Renderer* const pRenderer, const CommandBuffer* const pCommandBuffer);
extern void VulkanBeginCommandBuffer(const CommandBuffer* const pCommandBuffer);
extern void VulkanEndCommandBuffer(const CommandBuffer* const pCommandBuffer);

extern void VulkanBindRenderTarget(CommandBuffer* pCommandBuffer, const BindRenderTargetInfo* const pInfo);

extern void VulkanBindPipeline(CommandBuffer* pCommandBuffer, const Pipeline* const pPipeline);

extern void VulkanSetViewport(const CommandBuffer* const pCommandBuffer, const ViewportInfo* const pInfo);
extern void VulkanSetScissor(const CommandBuffer* const pCommandBuffer, const ScissorInfo* const pInfo);

extern void VulkanDraw(const CommandBuffer* const pCommandBuffer, const uint32_t vertexCount,
	const uint32_t instanceCount, const uint32_t firstVertex, const uint32_t firstInstance);

extern void VulkanDrawIndexed(const CommandBuffer* const pCommandBuffer, const uint32_t indexCount,
	const uint32_t instanceCount, const uint32_t firstIndex, const uint32_t vertexOffset, const uint32_t firstInstance);

extern void VulkanDispatch(const CommandBuffer* const pCommandBuffer, const uint32_t x, const uint32_t y, const uint32_t z);

extern void VulkanCreateFence(const Renderer* const pRenderer, Fence* pFence);
extern void VulkanDestroyFence(const Renderer* const pRenderer, Fence* pFence);
extern void VulkanWaitForFence(const Renderer* const pRenderer, Fence* pFence);

extern void VulkanCreateSemaphore(const Renderer* const pRenderer, Semaphore* pSemaphore);
extern void VulkanDestroySemaphore(const Renderer* const pRenderer, Semaphore* pSemaphore);

extern void VulkanAcquireNextImage(const Renderer* const pRenderer, const SwapChain* const pSwapChain,
	const Semaphore* const pSemaphore, uint32_t* pImageIndex);

extern void VulkanQueueSubmit(const QueueSubmitInfo* const pInfo);

extern void VulkanQueuePresent(const PresentInfo* const pInfo);

extern void VulkanCreateBuffer(const Renderer* const pRenderer, const BufferInfo* const pBufferInfo, Buffer* pBuffer);

extern void VulkanDestroyBuffer(const Renderer* const pRenderer, Buffer* pBuffer);

extern void VulkanMapMemory(const Renderer* const pRenderer, const Buffer* const buffer, void** ppData);

extern void VulkanUnmapMemory(const Renderer* const pRenderer, const Buffer* const pBuffer);

extern void VulkanBindVertexBuffer(const CommandBuffer* const pCommandBuffer, const uint32_t stride, const uint32_t bindingLocation,
	const uint32_t offset, const Buffer* const pBuffer);

extern void VulkanBindIndexBuffer(const CommandBuffer* const pCommandBuffer, const uint32_t offset, const IndexType indexType, const Buffer* const pBuffer);

extern void VulkanCreateTexture(const Renderer* const pRenderer, const TextureInfo* pInfo, Texture* pTexture);
extern void VulkanDestroyTexture(const Renderer* const pRenderer, Texture* pTexture);

extern void VulkanResourceBarrier(const CommandBuffer* const pCommandBuffer, const uint32_t numBarrierInfos, const BarrierInfo* const pBarrierInfos);

extern void VulkanCreateSampler(const Renderer* const pRenderer, const SamplerInfo* const pInfo, Sampler* pSampler);
extern void VulkanDestroySampler(const Renderer* const pRenderer, Sampler* pSampler);

extern void VulkanCreateDescriptorSet(const Renderer* const pRenderer, const DescriptorSetInfo* const pInfo, DescriptorSet* pDescriptorSet);
extern void VulkanDestroyDescriptorSet(DescriptorSet* pDescriptorSet);

extern void VulkanUpdateDescriptorSet(const Renderer* const pRenderer, const DescriptorSet* const  pDescriptorSet,
	const uint32_t index, const uint32_t numInfos, const UpdateDescriptorSetInfo* const pInfos);

extern void VulkanBindDescriptorSet(const CommandBuffer* const pCommandBuffer, const uint32_t index,
	const uint32_t firstSet, const DescriptorSet* const pDescriptorSet);

extern void VulkanBindRootConstants(const CommandBuffer* const pCommandBuffer, uint32_t numValues, uint32_t stride, const void* pData, uint32_t offset);

extern void VulkanInitUI(const Renderer* const pRenderer, const UIDesc* const pInfo);
extern void VulkanDestroyUI(const Renderer* const pRenderer);
extern void VulkanRenderUI(const CommandBuffer* const pCommandBuffer);

extern void VulkanSwapChainResize(const Renderer* const pRenderer, const SwapChainInfo* const pInfo, SwapChain* pSwapChain);

//CALLABLE
void (*InitRenderer)(Renderer* pRenderer, const char*);
void (*DestroyRenderer)(Renderer* pRenderer);

void (*CreateFence)(const Renderer* const pRenderer, Fence* pFence);
void (*DestroyFence)(const Renderer* const pRenderer, Fence* pFence);
void (*WaitForFence)(const Renderer* const pRenderer, Fence* pFence);

void (*CreateQueue)(const Renderer* const pRenderer, const QueueType type, Queue* pQueue);
void (*DestroyQueue)(const Renderer* const pRenderer, Queue* pQueue);
void (*WaitQueueIdle)(const Renderer* const pRenderer, Queue* pQueue);

void (*CreateRenderTarget)(const Renderer* const pRenderer, const RenderTargetInfo* const pInfo, RenderTarget* pRenderTarget);
void (*DestroyRenderTarget)(const Renderer* const pRenderer, RenderTarget* pRenderTarget);
void (*BindRenderTarget)(CommandBuffer* pCommandBuffer, const BindRenderTargetInfo* const pInfo);

void (*CreateSwapChain)(const Renderer* const pRenderer, const SwapChainInfo* const pInfo, SwapChain* pSwapChain);
void (*DestroySwapChain)(const Renderer* const pRenderer, SwapChain* pSwapChain);

void (*CreateShader)(const Renderer* const pRenderer, const ShaderInfo* const pInfo, Shader* pShader);
void (*DestroyShader)(const Renderer* const pRenderer, Shader* pShader);

void (*CreateRootSignature)(const Renderer* const pRenderer, const RootSignatureInfo* const pInfo, RootSignature* pRootSignature);
void (*DestroyRootSignature)(const Renderer* const pRenderer, RootSignature* pRootSignature);

void (*CreatePipeline)(const Renderer* const pRenderer, const PipelineInfo* const pInfo, Pipeline* pPipeline);
void (*DestroyPipeline)(const Renderer* const pRenderer, Pipeline* pPipeline);
void (*BindPipeline)(CommandBuffer* pCommandBuffer, const Pipeline* const pPipeline);

void (*CreateSemaphore)(const Renderer* const pRenderer, Semaphore* pSemaphore);
void (*DestroySemaphore)(const Renderer* const pRenderer, Semaphore* pSemaphore);

void (*CreateCommandBuffer)(const Renderer* const pRenderer, const QueueType type, CommandBuffer* pCommandBuffer);
void (*DestroyCommandBuffer)(const Renderer* const pRenderer, CommandBuffer* pCommandBuffer);
void (*ResetCommandBuffer)(const Renderer* const pRenderer, const CommandBuffer* const pCommandBuffer);
void (*BeginCommandBuffer)(const CommandBuffer* const pCommandBuffer);
void (*EndCommandBuffer)(const CommandBuffer* const pCommandBuffer);

void (*CreateBuffer)(const Renderer* const pRenderer, const BufferInfo* pInfo, Buffer* pBuffer);
void (*DestroyBuffer)(const Renderer* const pRenderer, Buffer* pBuffer);
void (*MapMemory)(const Renderer* const pRenderer, const Buffer* const pBuffer, void** ppData);
void (*UnmapMemory)(const Renderer* const pRenderer, const Buffer* const pBuffer);
void (*BindVertexBuffer)(const CommandBuffer* const pCommandBuffer, const uint32_t stride, const uint32_t bindingLocation,
	const uint32_t offset, const Buffer* const pVertexBuffer);
void (*BindIndexBuffer)(const CommandBuffer* const pCommandBuffer, const uint32_t offset,
	const IndexType indexType, const Buffer* const pIndexBuffer);

void (*CreateTexture)(const Renderer* const pRenderer, const TextureInfo* const pInfo, Texture* pTexture);
void (*DestroyTexture)(const Renderer* const pRenderer, Texture* pTexture);

void (*CreateSampler)(const Renderer* const pRenderer, const SamplerInfo* const pInfo, Sampler* pSampler);
void (*DestroySampler)(const Renderer* const pRenderer, Sampler* pSampler);

void (*CreateDescriptorSet)(const Renderer* const pRenderer, const DescriptorSetInfo* const pInfo, DescriptorSet* pDescriptorSet);
void (*DestroyDescriptorSet)(DescriptorSet* pDescriptorSet);
void (*UpdateDescriptorSet)(const Renderer* const pRenderer, const DescriptorSet* const  pDescriptorSet,
	const uint32_t index, const uint32_t numInfos, const UpdateDescriptorSetInfo* const pInfos);

void (*BindDescriptorSet)(const CommandBuffer* const pCommandBuffer, const uint32_t index,
	const uint32_t firstSet, const DescriptorSet* const pDescriptorSet);

void (*BindRootConstants)(const CommandBuffer* const pCommandBuffer, uint32_t numValues, uint32_t stride, const void* pData, uint32_t offset);

void (*AcquireNextImage)(const Renderer* const pRenderer, const SwapChain* const pSwapChain,
	const Semaphore* const pSemaphore, uint32_t* pImageIndex);

void (*SetViewport)(const CommandBuffer* const pCommandBuffer, const ViewportInfo* const pInfo);
void (*SetScissor)(const CommandBuffer* const pCommandBuffer, const ScissorInfo* const pInfo);
void (*DrawInstanced)(const CommandBuffer* const pCommandBuffer, const uint32_t vertexCount,
	const uint32_t instanceCount, const uint32_t firstVertex, const uint32_t firstInstance);
void (*DrawIndexedInstanced)(const CommandBuffer* const pCommandBuffer, const uint32_t indexCount,
	const uint32_t instanceCount, const uint32_t firstIndex, const uint32_t vertexOffset, const uint32_t firstInstance);
void (*Dispatch)(const CommandBuffer* const pCommandBuffer, const uint32_t x, const uint32_t y, const uint32_t z);

void (*QueueSubmit)(const QueueSubmitInfo* const pInfo);
void (*QueuePresent)(const PresentInfo* const pInfo);

void (*ResourceBarrier)(const CommandBuffer* const pCommandBuffer, const uint32_t numBarrierInfos, const BarrierInfo* const pBarrierInfos);

void (*InitUI)(const Renderer* const pRenderer, const UIDesc* const pInfo);
void (*DestroyUI)(const Renderer* const pRenderer);
void (*RenderUI)(const CommandBuffer* const pCommandBuffer);

void (*SwapChainResize)(const Renderer* const pRenderer, const SwapChainInfo* const pInfo, SwapChain* pSwapChain);

void InitSE()
{
	if (gRendererAPI == VULKAN)
	{
		InitRenderer = VulkanInitRenderer;
		DestroyRenderer = VulkanDestroyRenderer;

		CreateFence = VulkanCreateFence;
		DestroyFence = VulkanDestroyFence;
		WaitForFence = VulkanWaitForFence;

		CreateQueue = VulkanCreateQueue;
		DestroyQueue = VulkanDestroyQueue;
		WaitQueueIdle = VulkanWaitQueueIdle;

		CreateRenderTarget = VulkanCreateRenderTarget;
		DestroyRenderTarget = VulkanDestroyRenderTarget;
		BindRenderTarget = VulkanBindRenderTarget;

		CreateSwapChain = VulkanCreateSwapChain;
		DestroySwapChain = VulkanDestroySwapChain;

		CreateShader = VulkanCreateShader;
		DestroyShader = VulkanDestroyShader;

		CreateRootSignature = VulkanCreateRootSignature;
		DestroyRootSignature = VulkanDestroyRootSignature;

		CreatePipeline = VulkanCreatePipeline;
		DestroyPipeline = VulkanDestroyPipeline;
		BindPipeline = VulkanBindPipeline;

		CreateSemaphore = VulkanCreateSemaphore;
		DestroySemaphore = VulkanDestroySemaphore;

		CreateCommandBuffer = VulkanCreateCommandBuffer;
		DestroyCommandBuffer = VulkanDestroyCommandBuffer;
		ResetCommandBuffer = VulkanResetCommandBuffer;
		BeginCommandBuffer = VulkanBeginCommandBuffer;
		EndCommandBuffer = VulkanEndCommandBuffer;

		CreateBuffer = VulkanCreateBuffer;
		DestroyBuffer = VulkanDestroyBuffer;
		MapMemory = VulkanMapMemory;
		UnmapMemory = VulkanUnmapMemory;
		BindVertexBuffer = VulkanBindVertexBuffer;
		BindIndexBuffer = VulkanBindIndexBuffer;

		CreateTexture = VulkanCreateTexture;
		DestroyTexture = VulkanDestroyTexture;

		CreateSampler = VulkanCreateSampler;
		DestroySampler = VulkanDestroySampler;

		CreateDescriptorSet = VulkanCreateDescriptorSet;
		DestroyDescriptorSet = VulkanDestroyDescriptorSet;
		UpdateDescriptorSet = VulkanUpdateDescriptorSet;
		BindDescriptorSet = VulkanBindDescriptorSet;
		BindRootConstants = VulkanBindRootConstants;

		AcquireNextImage = VulkanAcquireNextImage;
		SetViewport = VulkanSetViewport;
		SetScissor = VulkanSetScissor;
		DrawInstanced = VulkanDraw;
		DrawIndexedInstanced = VulkanDrawIndexed;
		Dispatch = VulkanDispatch;

		QueueSubmit = VulkanQueueSubmit;
		QueuePresent = VulkanQueuePresent;

		ResourceBarrier = VulkanResourceBarrier;

		InitUI = VulkanInitUI;
		DestroyUI = VulkanDestroyUI;
		RenderUI = VulkanRenderUI;

		SwapChainResize = VulkanSwapChainResize;
	}
	else if (gRendererAPI == DIRECTX)
	{
		InitRenderer = DirectXInitRenderer;
		DestroyRenderer = DirectXDestroyRenderer;

		CreateFence = DirectXCreateFence;
		DestroyFence = DirectXDestroyFence;
		WaitForFence = DirectXWaitForFence;

		CreateQueue = DirectXCreateQueue;
		DestroyQueue = DirectXDestroyQueue;
		WaitQueueIdle = DirectXWaitQueueIdle;

		CreateRenderTarget = DirectXCreateRenderTarget;
		DestroyRenderTarget = DirectXDestroyRenderTarget;
		BindRenderTarget = DirectXBindRenderTarget;

		CreateSwapChain = DirectXCreateSwapChain;
		DestroySwapChain = DirectXDestroySwapChain;

		CreateShader = DirectXCreateShader;
		DestroyShader = DirectXDestroyShader;

		CreateRootSignature = DirectXCreateRootSignature;
		DestroyRootSignature = DirectXDestroyRootSignature;

		CreatePipeline = DirectXCreatePipeline;
		DestroyPipeline = DirectXDestroyPipeline;
		BindPipeline = DirectXBindPipeline;

		CreateSemaphore = DirectXCreateSemaphore;
		DestroySemaphore = DirectXDestroySemaphore;

		CreateCommandBuffer = DirectXCreateCommandBuffer;
		DestroyCommandBuffer = DirectXDestroyCommandBuffer;
		ResetCommandBuffer = DirectXResetCommandBuffer;
		BeginCommandBuffer = DirectXBeginCommandBuffer;
		EndCommandBuffer = DirectXEndCommandBuffer;

		CreateBuffer = DirectXCreateBuffer;
		DestroyBuffer = DirectXDestroyBuffer;
		MapMemory = DirectXMapMemory;
		UnmapMemory = DirectXUnmapMemory;
		BindVertexBuffer = DirectXBindVertexBuffer;
		BindIndexBuffer = DirectXBindIndexBuffer;

		CreateTexture = DirectXCreateTexture;
		DestroyTexture = DirectXDestroyTexture;

		CreateSampler = DirectXCreateSampler;
		DestroySampler = DirectXDestroySampler;

		CreateDescriptorSet = DirectXCreateDescriptorSet;
		DestroyDescriptorSet = DirectXDestroyDescriptorSet;
		UpdateDescriptorSet = DirectXUpdateDescriptorSet;
		BindDescriptorSet = DirectXBindDescriptorSet;

		BindRootConstants = DirectXBindRootConstants;

		AcquireNextImage = DirectXAcquireNextImage;
		SetViewport = DirectXSetViewport;
		SetScissor = DirectXSetScissor;
		DrawInstanced = DirectXDraw;
		DrawIndexedInstanced = DirectXDrawIndexed;
		Dispatch = DirectXDispatch;

		QueueSubmit = DirectXQueueSubmit;
		QueuePresent = DirectXQueuePresent;

		ResourceBarrier = DirectXResourceBarrier;

		InitUI = DirectXInitUI;
		DestroyUI = DirectXDestroyUI;
		RenderUI = DirectXRenderUI;

		SwapChainResize = DirectXSwapChainResize;
	}
}

void ExitSE()
{
	InitRenderer = nullptr;
	DestroyRenderer = nullptr;

	CreateFence = nullptr;
	DestroyFence = nullptr;
	WaitForFence = nullptr;

	CreateQueue = nullptr;
	DestroyQueue = nullptr;
	WaitQueueIdle = nullptr;

	CreateRenderTarget = nullptr;
	DestroyRenderTarget = nullptr;
	BindRenderTarget = nullptr;

	CreateSwapChain = nullptr;
	DestroySwapChain = nullptr;

	CreateShader = nullptr;
	DestroyShader = nullptr;

	CreateRootSignature = nullptr;
	DestroyRootSignature = nullptr;

	CreatePipeline = nullptr;
	DestroyPipeline = nullptr;
	BindPipeline = nullptr;

	CreateSemaphore = nullptr;
	DestroySemaphore = nullptr;

	CreateCommandBuffer = nullptr;
	DestroyCommandBuffer = nullptr;
	ResetCommandBuffer = nullptr;
	BeginCommandBuffer = nullptr;
	EndCommandBuffer = nullptr;

	CreateBuffer = nullptr;
	DestroyBuffer = nullptr;
	MapMemory = nullptr;
	UnmapMemory = nullptr;
	BindVertexBuffer = nullptr;
	BindIndexBuffer = nullptr;

	CreateTexture = nullptr;
	DestroyTexture = nullptr;

	CreateSampler = nullptr;
	DestroySampler = nullptr;

	CreateDescriptorSet = nullptr;
	DestroyDescriptorSet = nullptr;
	UpdateDescriptorSet = nullptr;
	BindDescriptorSet = nullptr;
	BindRootConstants = nullptr;

	AcquireNextImage = nullptr;
	SetViewport = nullptr;
	SetScissor = nullptr;
	DrawInstanced = nullptr;
	DrawIndexedInstanced = nullptr;
	Dispatch = nullptr;

	QueueSubmit = nullptr;
	QueuePresent = nullptr;

	ResourceBarrier = nullptr;

	InitUI = nullptr;
	DestroyUI = nullptr;
	RenderUI = nullptr;

	SwapChainResize = nullptr;
}

void OnRendererApiSwitch()
{
	gRendererAPI = (gRendererAPI == VULKAN) ? DIRECTX : VULKAN;
}

void NewFrameApi()
{
	if (gRendererAPI == VULKAN)
		ImGui_ImplVulkan_NewFrame();
	else
		ImGui_ImplDX12_NewFrame();
}
