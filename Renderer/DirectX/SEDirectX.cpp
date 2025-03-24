#include "../SERenderer.h"
#include <comdef.h>

extern "C" { __declspec(dllexport) extern const uint32_t D3D12SDKVersion = 615; }
extern "C" { __declspec(dllexport) extern const char* D3D12SDKPath = u8""; }


#ifndef DIRECTX_ERROR_CHECK
#define DIRECTX_ERROR_CHECK(x)																								\
{																															\
	HRESULT result = (x);																									\
	if (FAILED(result))																										\
	{																														\
		wchar_t errMsg[2048]{};																								\
		wchar_t file[2048]{};																								\
		_com_error err(result);																								\
		size_t convertedChars = 0;																							\
		mbstowcs_s(&convertedChars, file, 2048, __FILE__, _TRUNCATE);														\
		swprintf_s(errMsg, L"%s\nFailed in %s\nLine %d\nError = %s", L#x, file, __LINE__, err.ErrorMessage());				\
		MessageBox(nullptr, errMsg, L"Renderer Function Failed", MB_OK);													\
		exit(2);																											\
	}																														\
}																																
#endif

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(ptr)	\
	if(ptr)					\
	{						\
		ptr->Release();		\
		ptr = nullptr;		\
	}
#endif

#ifndef SAFE_FREE
#define SAFE_FREE(ptr)		\
	if(ptr)					\
	{						\
		free((void*)ptr);	\
		ptr = nullptr;		\
	}
#endif

typedef HRESULT(*PFN_D3D12_CREATE_DEVICE_FIXED)(_In_opt_ void* pAdapter, D3D_FEATURE_LEVEL MinimumFeatureLevel, _In_ REFIID riid,
	_COM_Outptr_opt_ void** ppDevice);

typedef HRESULT(WINAPI* PFN_D3D12_SerializeVersionedRootSignature)(const D3D12_VERSIONED_ROOT_SIGNATURE_DESC*, ID3DBlob**, ID3DBlob**);
typedef HRESULT(WINAPI* PFN_DXGI_CreateDXGIFactory2)(UINT, REFIID riid, void** ppFactory);
typedef HRESULT(WINAPI* PFN_DXGI_DXGIGetDebugInterface1)(UINT Flags, REFIID riid, _COM_Outptr_ void** pDebug);

static bool                                      gD3D12dllInited = false;
static PFN_D3D12_CREATE_DEVICE_FIXED             gPfnCreateDevice = nullptr;
static PFN_D3D12_GET_DEBUG_INTERFACE             gPfnGetDebugInterface = nullptr;
static PFN_D3D12_SerializeVersionedRootSignature gPfnSerializeVersionedRootSignature = nullptr;
static PFN_DXGI_CreateDXGIFactory2               gPfnCreateDXGIFactory2 = nullptr;
static PFN_DXGI_DXGIGetDebugInterface1           gPfnDXGIGetDebugInterface1 = nullptr;
static HMODULE                                   gD3D12dll = nullptr;
static HMODULE                                   gDXGIdll = nullptr;

void Exit_d3d12_dll()
{
	if (gDXGIdll)
	{
		gPfnCreateDXGIFactory2 = nullptr;
		gPfnDXGIGetDebugInterface1 = nullptr;
		FreeLibrary(gDXGIdll);
		gDXGIdll = nullptr;
	}

	if (gD3D12dll)
	{
		gPfnCreateDevice = nullptr;
		gPfnGetDebugInterface = nullptr;
		gPfnSerializeVersionedRootSignature = nullptr;
		FreeLibrary(gD3D12dll);
;		gD3D12dll = nullptr;
	}

	gD3D12dllInited = false;
}

bool Init_d3d12_dll()
{
	if (gD3D12dllInited)
		return true;

	gD3D12dll = LoadLibraryExA("d3d12.dll", NULL, LOAD_LIBRARY_SEARCH_SYSTEM32);
	if (gD3D12dll)
	{
		gPfnCreateDevice = (PFN_D3D12_CREATE_DEVICE_FIXED)(GetProcAddress(gD3D12dll, "D3D12CreateDevice"));
		gPfnGetDebugInterface = (PFN_D3D12_GET_DEBUG_INTERFACE)(GetProcAddress(gD3D12dll, "D3D12GetDebugInterface"));
		gPfnSerializeVersionedRootSignature =
			(PFN_D3D12_SerializeVersionedRootSignature)(GetProcAddress(gD3D12dll, "D3D12SerializeVersionedRootSignature"));
	}

	gDXGIdll = LoadLibraryExA("dxgi.dll", NULL, LOAD_LIBRARY_SEARCH_SYSTEM32);
	if (gDXGIdll)
	{
		gPfnCreateDXGIFactory2 = (PFN_DXGI_CreateDXGIFactory2)(GetProcAddress(gDXGIdll, "CreateDXGIFactory2"));
		gPfnDXGIGetDebugInterface1 = (PFN_DXGI_DXGIGetDebugInterface1)(GetProcAddress(gDXGIdll, "DXGIGetDebugInterface1"));
	}

	gD3D12dllInited = gDXGIdll && gD3D12dll;
	if (!gD3D12dllInited)
		Exit_d3d12_dll();

	return gD3D12dllInited;
}

HRESULT WINAPI CreateDevice_d3d12_dll(void* pAdapter, D3D_FEATURE_LEVEL MinimumFeatureLevel, REFIID riid, void** ppDevice)
{                                                                                                                     
	if (gPfnCreateDevice)
		return gPfnCreateDevice(pAdapter, MinimumFeatureLevel, riid, ppDevice);
	return D3D12_ERROR_ADAPTER_NOT_FOUND;
}

HRESULT WINAPI GetDebugInterface_d3d12_dll(REFIID riid, void** ppvDebug)
{                                                                      
	if (gPfnGetDebugInterface)
		return gPfnGetDebugInterface(riid, ppvDebug);
	return E_FAIL;
}

static HRESULT SerializeVersionedRootSignature_d3d12_dll(const D3D12_VERSIONED_ROOT_SIGNATURE_DESC * pRootSignature, ID3DBlob * *ppBlob,
	ID3DBlob * *ppErrorBlob)
{
	if (gPfnSerializeVersionedRootSignature)
		return gPfnSerializeVersionedRootSignature(pRootSignature, ppBlob, ppErrorBlob);
	return E_NOTIMPL;
}

static HRESULT WINAPI CreateDXGIFactory2_d3d12_dll(UINT Flags, REFIID riid, void** ppFactory)
{                                                                                           
	if (gPfnCreateDXGIFactory2)
		return gPfnCreateDXGIFactory2(Flags, riid, ppFactory);
	return DXGI_ERROR_UNSUPPORTED;
}

HRESULT WINAPI DXGIGetDebugInterface1_d3d12_dll(UINT Flags, REFIID riid, void** ppFactory)
{                                                                                     
	if (gPfnDXGIGetDebugInterface1)
		return gPfnDXGIGetDebugInterface1(Flags, riid, ppFactory);
	return DXGI_ERROR_UNSUPPORTED;
}

#if defined(_DEBUG)
void DirectXEnableDebugLayer(Renderer * pRenderer, uint32_t* dxgiFactoryFlags)
{
	ID3D12Debug1* pDebug = nullptr;
	DIRECTX_ERROR_CHECK(GetDebugInterface_d3d12_dll(IID_PPV_ARGS(&pRenderer->dx.debug)));
	pRenderer->dx.debug->EnableDebugLayer();
	pRenderer->dx.debug->QueryInterface(&pDebug);
	pDebug->SetEnableGPUBasedValidation(true);

	SAFE_RELEASE(pDebug);

	*dxgiFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
}
#endif

void DirectXFindAdapter(Renderer* pRenderer)
{
	D3D_FEATURE_LEVEL featureLevels[5] = 
	{
		D3D_FEATURE_LEVEL_12_2,
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

	IDXGIAdapter4* adapter = nullptr;
	for (uint32_t i = 0;
		pRenderer->dx.factory->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adapter)) != DXGI_ERROR_NOT_FOUND;
		++i)
	{
		DXGI_ADAPTER_DESC3 desc{};
		adapter->GetDesc3(&desc);

		if ((desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) == 0)
		{
			DXGI_ADAPTER_DESC3 desc{};
			adapter->GetDesc3(&desc);

			wchar_t msg[1024]{};
			wchar_t format[] = L"Description = %s\nVendorID = %d\nSubSysId = %d\nRevision = %d\n"
				"Dedicated Video Memory = %lld\nDedicated System Memory = %lld\nShared System Memory = %lld\n";
			swprintf_s(msg, format, desc.Description, desc.VendorId, desc.SubSysId, desc.Revision, desc.DedicatedVideoMemory,
				desc.DedicatedSystemMemory, desc.SharedSystemMemory);

			OutputDebugString(msg);

			for (uint32_t level = 0; level < 5; ++level)
			{
				if (SUCCEEDED(CreateDevice_d3d12_dll(adapter, featureLevels[level], _uuidof(ID3D12Device), nullptr)))
				{
					DIRECTX_ERROR_CHECK(adapter->QueryInterface(IID_PPV_ARGS(&pRenderer->dx.adapter)));
					pRenderer->dx.featureLevel = featureLevels[level];
					SAFE_RELEASE(adapter);
					return;
				}
			}
		}

		SAFE_RELEASE(adapter);
	}

	MessageBox(nullptr, L"Couldn't find a GPU with a supported feature level. Exiting program.", L"FindAdapter error.", MB_OK);
	exit(-1);
}

#if defined(_DEBUG)
void DirectXEnableInfoQueue(Renderer* pRenderer)
{
	if (pRenderer->dx.infoQueue != nullptr)
	{
		SAFE_RELEASE(pRenderer->dx.infoQueue);
	}

	if (SUCCEEDED(pRenderer->dx.device->QueryInterface(IID_PPV_ARGS(&pRenderer->dx.infoQueue))))
	{
		pRenderer->dx.infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		pRenderer->dx.infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		pRenderer->dx.infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, false);
		pRenderer->dx.infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_INFO, false);
		pRenderer->dx.infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_MESSAGE, false);
	}

	D3D12_MESSAGE_ID ignoreMessageIDs[] =
	{
		D3D12_MESSAGE_ID_CREATERESOURCE_STATE_IGNORED,
		D3D12_MESSAGE_ID_INVALID_SUBRESOURCE_STATE
	};

	D3D12_INFO_QUEUE_FILTER filter = {};
	filter.DenyList.NumIDs = _countof(ignoreMessageIDs);
	filter.DenyList.pIDList = ignoreMessageIDs;
	pRenderer->dx.infoQueue->AddStorageFilterEntries(&filter);
}
#endif

struct DirectXDescriptorHeapInfo
{
	D3D12_DESCRIPTOR_HEAP_TYPE type;
	uint32_t numCpuDescriptors;
	uint32_t numGpuDescriptors;
	bool createCpuHeap;
	bool createGpuHeap;
};

struct DirectXDescriptorHeap
{
	D3D12_DESCRIPTOR_HEAP_TYPE type;
	ID3D12DescriptorHeap* cpuHeap;
	ID3D12DescriptorHeap* gpuHeap;
	uint32_t descriptorSize;
	uint32_t numCpuDescriptors;
	uint32_t numGpuDescriptors;
	uint32_t* freeCpuIndices;
	uint32_t* freeGpuIndices;
};

void DirectXCreateDescriptorHeap(const Renderer* const pRenderer, const DirectXDescriptorHeapInfo* const info, DirectXDescriptorHeap* heap)
{
	if (info->createCpuHeap == true)
	{
		D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
		heapDesc.Type = info->type;
		heapDesc.NumDescriptors = info->numCpuDescriptors;
		heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		heapDesc.NodeMask = 0;
		DIRECTX_ERROR_CHECK(pRenderer->dx.device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&heap->cpuHeap)));
	}

	if (info->createGpuHeap == true)
	{
		D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
		heapDesc.Type = info->type;
		heapDesc.NumDescriptors = info->numGpuDescriptors;
		heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		heapDesc.NodeMask = 0;
		DIRECTX_ERROR_CHECK(pRenderer->dx.device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&heap->gpuHeap)));
	}
	arrsetcap(heap->freeCpuIndices, 16);
	arrsetcap(heap->freeGpuIndices, 16);
	heap->descriptorSize = pRenderer->dx.device->GetDescriptorHandleIncrementSize(info->type);
	heap->type = info->type;
	heap->numCpuDescriptors = 0;
	heap->numGpuDescriptors = 0;
}

void DirectXDestroyDescriptorHeap(DirectXDescriptorHeap* heap)
{
	SAFE_RELEASE(heap->cpuHeap);
	SAFE_RELEASE(heap->gpuHeap);
	arrfree(heap->freeCpuIndices);
	arrfree(heap->freeGpuIndices);

	heap->descriptorSize = 0;
	heap->numCpuDescriptors = 0;
	heap->numGpuDescriptors = 0;
}

enum ViewType
{
	VIEW_TYPE_RTV,
	VIEW_TYPE_DSV,
	VIEW_TYPE_CBV,
	VIEW_TYPE_SRV,
	VIEW_TYPE_UAV,
	VIEW_TYPE_SAMPLER
};

struct DirectXDescriptroHeapAllocateInfo
{
	ViewType type;

	union
	{
		D3D12_RENDER_TARGET_VIEW_DESC* pRtvDesc;
		D3D12_DEPTH_STENCIL_VIEW_DESC* pDsvDesc;
		D3D12_CONSTANT_BUFFER_VIEW_DESC* pCbvDesc;
		D3D12_SHADER_RESOURCE_VIEW_DESC* pSrvDesc;
		D3D12_UNORDERED_ACCESS_VIEW_DESC* pUavDesc;
		D3D12_SAMPLER_DESC* pSamplerDesc;
	};

	union
	{
		RenderTarget* pRenderTarget;
		Buffer* pBuffer;
		Texture* pTexture;
		Sampler* pSampler;
	};

	bool allocateOnGpuHeap;
	bool isBuffer; //Only used when allocateOnGpuHeap == true
};

void DirectXDescriptorHeapAllocate(const Renderer* const pRenderer, DirectXDescriptroHeapAllocateInfo* pInfo, DirectXDescriptorHeap* pHeap, 
	uint32_t* outIndex)
{
	if (pInfo->allocateOnGpuHeap == false)
	{
		uint32_t index = 0;
		if (arrlenu(pHeap->freeCpuIndices) > 0)
		{
			index = arrpop(pHeap->freeCpuIndices);
		}
		else
		{
			index = pHeap->numCpuDescriptors;
		}

		if (outIndex != nullptr)
			*outIndex = index;

		D3D12_CPU_DESCRIPTOR_HANDLE handle = pHeap->cpuHeap->GetCPUDescriptorHandleForHeapStart();
		handle.ptr += (index * pHeap->descriptorSize);
		if (pInfo->type == VIEW_TYPE_RTV)
		{
			pRenderer->dx.device->CreateRenderTargetView(pInfo->pRenderTarget->texture.dx.resource, pInfo->pRtvDesc, handle);
			pInfo->pRenderTarget->dx.descriptorId = index;
		}
		else if (pInfo->type == VIEW_TYPE_DSV)
		{
			pRenderer->dx.device->CreateDepthStencilView(pInfo->pRenderTarget->texture.dx.resource, pInfo->pDsvDesc, handle);
			pInfo->pRenderTarget->dx.descriptorId = index;
		}
		else if (pInfo->type == VIEW_TYPE_CBV)
		{
			pRenderer->dx.device->CreateConstantBufferView(pInfo->pCbvDesc, handle);
			pInfo->pBuffer->dx.cpuCbDescriptorId = index;
		}
		else if (pInfo->type == VIEW_TYPE_SRV)
		{
			if (pInfo->pSrvDesc->ViewDimension == D3D12_SRV_DIMENSION_BUFFER)
			{
				pRenderer->dx.device->CreateShaderResourceView(pInfo->pBuffer->dx.resource, pInfo->pSrvDesc, handle);
				pInfo->pBuffer->dx.cpuSrvDescriptorId = index;
			}
			else //TEXTURE
			{
				pRenderer->dx.device->CreateShaderResourceView(pInfo->pTexture->dx.resource, pInfo->pSrvDesc, handle);
				pInfo->pTexture->dx.cpuSrvDescriptorId = index;
			}
		}
		else if(pInfo->type == VIEW_TYPE_UAV)
		{
			if (pInfo->pUavDesc->ViewDimension == D3D12_UAV_DIMENSION_BUFFER)
			{
				pRenderer->dx.device->CreateUnorderedAccessView(pInfo->pBuffer->dx.resource, nullptr, pInfo->pUavDesc, handle);
				pInfo->pBuffer->dx.cpuUavDescriptorId = index;
			}
			else //TEXTURE
			{
				pRenderer->dx.device->CreateUnorderedAccessView(pInfo->pTexture->dx.resource, nullptr, pInfo->pUavDesc, handle);
				pInfo->pTexture->dx.cpuUavDescriptorId = index;
			}
		}
		else //VIEW_TYPE_SAMPLER
		{
			pRenderer->dx.device->CreateSampler(pInfo->pSamplerDesc, handle);
			pInfo->pSampler->dx.cpuDescriptorId = index;
		}

		++pHeap->numCpuDescriptors;
	}
	else //allocateOnGpuHeap == true
	{
		uint32_t index = 0;
		if (arrlenu(pHeap->freeGpuIndices) > 0)
		{
			index = arrpop(pHeap->freeGpuIndices);
		}
		else
		{
			index = pHeap->numGpuDescriptors;
		}

		if (outIndex != nullptr)
			*outIndex = index;

		D3D12_CPU_DESCRIPTOR_HANDLE srcHandle = pHeap->cpuHeap->GetCPUDescriptorHandleForHeapStart();

		D3D12_CPU_DESCRIPTOR_HANDLE dstHandle = pHeap->gpuHeap->GetCPUDescriptorHandleForHeapStart();
		dstHandle.ptr += (index * pHeap->descriptorSize);

		if (pInfo->type == VIEW_TYPE_CBV)
		{
			srcHandle.ptr += (pInfo->pBuffer->dx.cpuCbDescriptorId * pHeap->descriptorSize);
			pInfo->pBuffer->dx.gpuCbDescriptorId = index;
		}
		else if (pInfo->type == VIEW_TYPE_SRV)
		{
			if (pInfo->isBuffer)
			{
				srcHandle.ptr += (pInfo->pBuffer->dx.cpuSrvDescriptorId * pHeap->descriptorSize);
				pInfo->pBuffer->dx.gpuSrvDescriptorId = index;
			}
			else //TEXTURE
			{
				srcHandle.ptr += (pInfo->pTexture->dx.cpuSrvDescriptorId * pHeap->descriptorSize);
				pInfo->pTexture->dx.gpuSrvDescriptorId = index;
			}
		}
		else if (pInfo->type == VIEW_TYPE_UAV)
		{
			if (pInfo->isBuffer)
			{
				srcHandle.ptr += (pInfo->pBuffer->dx.cpuUavDescriptorId * pHeap->descriptorSize);
				pInfo->pBuffer->dx.gpuUavDescriptorId = index;
			}
			else //TEXTURE
			{
				srcHandle.ptr += (pInfo->pTexture->dx.cpuUavDescriptorId * pHeap->descriptorSize);
				pInfo->pTexture->dx.gpuUavDescriptorId = index;
			}
		}
		else //VIEW_TYPE_SAMPLER
		{
			srcHandle.ptr += (pInfo->pSampler->dx.cpuDescriptorId * pHeap->descriptorSize);
			pInfo->pSampler->dx.gpuDescriptorId = index;
		}

		if(pInfo->type != VIEW_TYPE_SAMPLER)
			pRenderer->dx.device->CopyDescriptorsSimple(1, dstHandle, srcHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		else
			pRenderer->dx.device->CopyDescriptorsSimple(1, dstHandle, srcHandle, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
		++pHeap->numGpuDescriptors;
	}
}

void DirectXDescriptorHeapFree(DirectXDescriptorHeap* heap, uint32_t cpuIndex, uint32_t gpuIndex)
{
	arrpush(heap->freeCpuIndices, cpuIndex);
	--heap->numCpuDescriptors;

	if (heap->type == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV || heap->type == D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER)
	{
		arrpush(heap->freeGpuIndices, gpuIndex);
		--heap->numGpuDescriptors;
	}
}

#define NUM_DESCRIPTORS_RTV 512
#define NUM_DESCRIPTORS_DSV 512
#define NUM_DESCRIPTORS_CBV_SRV_UAV 1024 * 256
#define NUM_DESCRIPTORS_SAMPLER 2048

DirectXDescriptorHeap gRtvHeap;
DirectXDescriptorHeap gDsvHeap;
DirectXDescriptorHeap gCbvSrvUavHeap;
DirectXDescriptorHeap gSamplerHeap;

void DirectXCreateDescriptorHeaps(const Renderer* const pRenderer)
{
	DirectXDescriptorHeapInfo heapInfo{};
	heapInfo.type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	heapInfo.numCpuDescriptors = NUM_DESCRIPTORS_RTV;
	heapInfo.createCpuHeap = true;
	heapInfo.createGpuHeap = false;
	DirectXCreateDescriptorHeap(pRenderer, &heapInfo, &gRtvHeap);

	heapInfo.type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	heapInfo.numCpuDescriptors = NUM_DESCRIPTORS_DSV;
	heapInfo.createCpuHeap = true;
	heapInfo.createGpuHeap = false;
	DirectXCreateDescriptorHeap(pRenderer, &heapInfo, &gDsvHeap);

	heapInfo.type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	heapInfo.numCpuDescriptors = NUM_DESCRIPTORS_CBV_SRV_UAV;
	heapInfo.numGpuDescriptors = D3D12_MAX_SHADER_VISIBLE_DESCRIPTOR_HEAP_SIZE_TIER_1;
	heapInfo.createCpuHeap = true;
	heapInfo.createGpuHeap = true;
	DirectXCreateDescriptorHeap(pRenderer, &heapInfo, &gCbvSrvUavHeap);

	heapInfo.type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
	heapInfo.numCpuDescriptors = NUM_DESCRIPTORS_SAMPLER;
	heapInfo.numGpuDescriptors = D3D12_MAX_SHADER_VISIBLE_SAMPLER_HEAP_SIZE;
	heapInfo.createCpuHeap = true;
	heapInfo.createGpuHeap = true;
	DirectXCreateDescriptorHeap(pRenderer, &heapInfo, &gSamplerHeap);
}

void DirectXDestroyDescriptorHeaps()
{
	DirectXDestroyDescriptorHeap(&gRtvHeap);
	DirectXDestroyDescriptorHeap(&gDsvHeap);
	DirectXDestroyDescriptorHeap(&gCbvSrvUavHeap);
	DirectXDestroyDescriptorHeap(&gSamplerHeap);
}

void DirectXInitUI(const Renderer* const pRenderer, const UIDesc* const pInfo)
{
	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = gCbvSrvUavHeap.gpuHeap->GetCPUDescriptorHandleForHeapStart();
	cpuHandle.ptr += (gCbvSrvUavHeap.numGpuDescriptors * gCbvSrvUavHeap.descriptorSize);

	D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = gCbvSrvUavHeap.gpuHeap->GetGPUDescriptorHandleForHeapStart();
	gpuHandle.ptr += (gCbvSrvUavHeap.numGpuDescriptors * gCbvSrvUavHeap.descriptorSize);
	++gCbvSrvUavHeap.numGpuDescriptors;

	ImGui_ImplDX12_InitInfo initInfo{};
	initInfo.Device = pRenderer->dx.device;
	initInfo.CommandQueue = pInfo->pQueue->dx.queue;
	initInfo.NumFramesInFlight = pInfo->numFrames;
	initInfo.RTVFormat = (DXGI_FORMAT)TinyImageFormat_ToDXGI_FORMAT(pInfo->renderTargetFormat);
	initInfo.DSVFormat = (DXGI_FORMAT)TinyImageFormat_ToDXGI_FORMAT(pInfo->depthFormat);
	initInfo.SrvDescriptorHeap = gCbvSrvUavHeap.gpuHeap;
	initInfo.LegacySingleSrvCpuDescriptor = cpuHandle;
	initInfo.LegacySingleSrvGpuDescriptor = gpuHandle;
	ImGui_ImplDX12_Init(&initInfo);
}

extern void DirectXDestroyUI(const Renderer* const pRenderer)
{
	ImGui_ImplDX12_Shutdown();
}

void DirectXRenderUI(const CommandBuffer* const pCommandBuffer)
{
	ImGui::Render();
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), pCommandBuffer->dx.commandList);
}

void DirectXCreateFence(const Renderer* const pRenderer, Fence* pFence)
{
	DIRECTX_ERROR_CHECK(pRenderer->dx.device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&pFence->dx.fence)));
	pFence->dx.fenceValue = 0;
	pFence->dx.fenceEvent = CreateEvent(nullptr, false, false, nullptr);
	if (pFence->dx.fenceEvent == nullptr)
	{
		DIRECTX_ERROR_CHECK(HRESULT_FROM_WIN32(GetLastError()));
	}
}

void DirectXDestroyFence(const Renderer* const pRenderer, Fence* pFence)
{
	CloseHandle(pFence->dx.fenceEvent);
	SAFE_RELEASE(pFence->dx.fence);
}

void DirectXWaitForFence(const Renderer* const pRenderer, Fence* pFence)
{
	DIRECTX_ERROR_CHECK(pFence->dx.fence->SetEventOnCompletion(pFence->dx.fenceValue, pFence->dx.fenceEvent));
	WaitForSingleObjectEx(pFence->dx.fenceEvent, INFINITE, false);
	++pFence->dx.fenceValue;
}

void DirectXCreateSemaphore(const Renderer* const pRenderer, Semaphore* pSemaphore)
{

}

void DirectXDestroySemaphore(const Renderer* const pRenderer, Semaphore* pSemaphore)
{

}

void DirectXCreateCommandBuffer(const Renderer* const pRenderer, const QueueType type, CommandBuffer* pCommandBuffer)
{
	D3D12_COMMAND_LIST_TYPE listType{};
	switch (type)
	{
	case QUEUE_TYPE_GRAPHICS:
		listType = D3D12_COMMAND_LIST_TYPE_DIRECT;
		break;

	case QUEUE_TYPE_COMPUTE:
		listType = D3D12_COMMAND_LIST_TYPE_COMPUTE;
		break;
	}

	DIRECTX_ERROR_CHECK(pRenderer->dx.device->CreateCommandAllocator(listType, IID_PPV_ARGS(&pCommandBuffer->dx.commandAllocator)));

	DIRECTX_ERROR_CHECK(pRenderer->dx.device->CreateCommandList(0, listType,
		pCommandBuffer->dx.commandAllocator, nullptr, IID_PPV_ARGS(&pCommandBuffer->dx.commandList)));

	DIRECTX_ERROR_CHECK(pCommandBuffer->dx.commandList->Close());

	DirectXCreateFence(pRenderer, &pCommandBuffer->fence);
	DirectXCreateSemaphore(pRenderer, &pCommandBuffer->semaphore);
}

void DirectXDestroyCommandBuffer(const Renderer* const pRenderer, CommandBuffer* pCommandBuffer)
{
	DirectXDestroyFence(pRenderer, &pCommandBuffer->fence);
	SAFE_RELEASE(pCommandBuffer->dx.commandList);
	SAFE_RELEASE(pCommandBuffer->dx.commandAllocator);
}

void DirectXResetCommandBuffer(const Renderer* const pRenderer, const CommandBuffer* const pCommandBuffer)
{
	DIRECTX_ERROR_CHECK(pCommandBuffer->dx.commandAllocator->Reset());
}

void DirectXBeginCommandBuffer(const CommandBuffer* const pCommandBuffer)
{
	DIRECTX_ERROR_CHECK(pCommandBuffer->dx.commandList->Reset(pCommandBuffer->dx.commandAllocator, nullptr));

	ID3D12DescriptorHeap* heaps[] = { gCbvSrvUavHeap.gpuHeap, gSamplerHeap.gpuHeap };
	pCommandBuffer->dx.commandList->SetDescriptorHeaps(2, heaps);
}

void DirectXEndCommandBuffer(const CommandBuffer* const pCommandBuffer)
{
	DIRECTX_ERROR_CHECK(pCommandBuffer->dx.commandList->Close());
}

void DirectXCreateQueue(const Renderer* const pRenderer, const QueueType type, Queue* pQueue)
{
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	switch (type)
	{
	case QUEUE_TYPE_GRAPHICS:
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		break;

	case QUEUE_TYPE_COMPUTE:
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
		break;
	}

	queueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.NodeMask = 0;

	DIRECTX_ERROR_CHECK(pRenderer->dx.device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&pQueue->dx.queue)));

	DirectXCreateFence(pRenderer, &pQueue->dx.fence);
}

void DirectXDestroyQueue(const Renderer* const pRenderer, Queue* pQueue)
{
	DirectXDestroyFence(pRenderer, &pQueue->dx.fence);
	SAFE_RELEASE(pQueue->dx.queue);
}

struct DirectXCopyEngine
{
	Queue copyQueue;
	CommandBuffer copyCommandBuffer;
	Buffer stagingBuffer;
}gDirectXCopyEngine;

void DirectXCopyEngineInit(const Renderer* const pRenderer)
{
	D3D12_COMMAND_QUEUE_DESC copyQueueDesc = {};
	copyQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
	copyQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	copyQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	copyQueueDesc.NodeMask = 0;

	DIRECTX_ERROR_CHECK(pRenderer->dx.device->CreateCommandQueue(&copyQueueDesc, IID_PPV_ARGS(&gDirectXCopyEngine.copyQueue.dx.queue)));

	DirectXCreateFence(pRenderer, &gDirectXCopyEngine.copyCommandBuffer.fence);
	++gDirectXCopyEngine.copyCommandBuffer.fence.dx.fenceValue;

	DIRECTX_ERROR_CHECK(pRenderer->dx.device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COPY, 
		IID_PPV_ARGS(&gDirectXCopyEngine.copyCommandBuffer.dx.commandAllocator)));

	DIRECTX_ERROR_CHECK(pRenderer->dx.device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_COPY,
		gDirectXCopyEngine.copyCommandBuffer.dx.commandAllocator, nullptr, IID_PPV_ARGS(&gDirectXCopyEngine.copyCommandBuffer.dx.commandList)));

	DIRECTX_ERROR_CHECK(gDirectXCopyEngine.copyCommandBuffer.dx.commandList->Close());
}

void DirectXCopyEngineDestroy(const Renderer* const pRenderer)
{
	DirectXDestroyCommandBuffer(pRenderer, &gDirectXCopyEngine.copyCommandBuffer);
	DirectXDestroyQueue(pRenderer, &gDirectXCopyEngine.copyQueue);
}

void DirectXBeginCopyEngineCommandList(const Renderer* const pRenderer)
{
	DIRECTX_ERROR_CHECK(gDirectXCopyEngine.copyCommandBuffer.dx.commandAllocator->Reset());
	DIRECTX_ERROR_CHECK(gDirectXCopyEngine.copyCommandBuffer.dx.commandList->Reset(
		gDirectXCopyEngine.copyCommandBuffer.dx.commandAllocator, nullptr));
}

void DirectXEndCopyEngineCommandList()
{
	DirectXEndCommandBuffer(&gDirectXCopyEngine.copyCommandBuffer);
}

void DirectXQueueSubmit(const QueueSubmitInfo* const pInfo)
{
	ID3D12CommandList* ppCommandLists[] = { pInfo->pCommandBuffer->dx.commandList };
	pInfo->pQueue->dx.queue->ExecuteCommandLists(1, ppCommandLists);

	if (pInfo->pFence != nullptr)
		pInfo->pQueue->dx.queue->Signal(pInfo->pFence->dx.fence, pInfo->pFence->dx.fenceValue);
}

void DirectXCopyEngineSubmit(const Renderer* const pRenderer)
{
	QueueSubmitInfo submitInfo{};
	submitInfo.pQueue = &gDirectXCopyEngine.copyQueue;
	submitInfo.numWaitSemaphores = 0;
	submitInfo.waitSemaphores = nullptr;
	submitInfo.numSignalSemaphores = 0;
	submitInfo.signalSemaphores = nullptr;
	submitInfo.pFence = &gDirectXCopyEngine.copyCommandBuffer.fence;
	submitInfo.pCommandBuffer = &gDirectXCopyEngine.copyCommandBuffer;
	DirectXQueueSubmit(&submitInfo);

	DirectXWaitForFence(pRenderer, &gDirectXCopyEngine.copyCommandBuffer.fence);
}

void DirectXInitRenderer(Renderer* pRenderer, const char* appName)
{
	Init_d3d12_dll();

	uint32_t dxgiFactoryFlags = 0;
#if defined(_DEBUG)
	DirectXEnableDebugLayer(pRenderer, &dxgiFactoryFlags);
#endif

	DIRECTX_ERROR_CHECK(CreateDXGIFactory2_d3d12_dll(dxgiFactoryFlags, IID_PPV_ARGS(&pRenderer->dx.factory)));

	DirectXFindAdapter(pRenderer);
	DIRECTX_ERROR_CHECK(CreateDevice_d3d12_dll(pRenderer->dx.adapter, pRenderer->dx.featureLevel, IID_PPV_ARGS(&pRenderer->dx.device)));

	D3D12MA::ALLOCATOR_DESC allocatorDesc{};
	allocatorDesc.pDevice = pRenderer->dx.device;
	allocatorDesc.pAdapter = pRenderer->dx.adapter;
	allocatorDesc.PreferredBlockSize = 0;
	allocatorDesc.pAllocationCallbacks = nullptr;
	allocatorDesc.Flags = D3D12MA::ALLOCATOR_FLAG_MSAA_TEXTURES_ALWAYS_COMMITTED |
		D3D12MA::ALLOCATOR_FLAG_DEFAULT_POOLS_NOT_ZEROED;

	DIRECTX_ERROR_CHECK(D3D12MA::CreateAllocator(&allocatorDesc, &pRenderer->dx.allocator));


#if defined(_DEBUG)
	DirectXEnableInfoQueue(pRenderer);
	pRenderer->dx.infoQueue->SetMuteDebugOutput(false);
#endif

	DirectXCreateDescriptorHeaps(pRenderer);

	DirectXCopyEngineInit(pRenderer);

	DirectXCreateQueue(pRenderer, QUEUE_TYPE_GRAPHICS, &pRenderer->queue);
	DirectXCreateCommandBuffer(pRenderer, QUEUE_TYPE_GRAPHICS, &pRenderer->commandBuffer);
}

void DirectXDestroyRenderer(Renderer* pRenderer)
{
	DirectXDestroyCommandBuffer(pRenderer, &pRenderer->commandBuffer);
	DirectXDestroyQueue(pRenderer, &pRenderer->queue);

	DirectXCopyEngineDestroy(pRenderer);
	DirectXDestroyDescriptorHeaps();


#if defined(_DEBUG)
	//Muting so when switching to Vulkan from DirectX in debug mode, there are d3d12 messages.
	pRenderer->dx.infoQueue->SetMuteDebugOutput(true);
	SAFE_RELEASE(pRenderer->dx.infoQueue);
#endif

	SAFE_RELEASE(pRenderer->dx.allocator);
	SAFE_RELEASE(pRenderer->dx.device);
	SAFE_RELEASE(pRenderer->dx.adapter);
	SAFE_RELEASE(pRenderer->dx.factory);

#if defined(_DEBUG)
	pRenderer->dx.debug->DisableDebugLayer();
	SAFE_RELEASE(pRenderer->dx.debug);

	IDXGIDebug1* dxgiDebug = nullptr;
	if (SUCCEEDED(DXGIGetDebugInterface1_d3d12_dll(0, IID_PPV_ARGS(&dxgiDebug))))
	{
		dxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL,
			DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_DETAIL | DXGI_DEBUG_RLO_SUMMARY | DXGI_DEBUG_RLO_IGNORE_INTERNAL));
		SAFE_RELEASE(dxgiDebug);
	}
#endif

	Exit_d3d12_dll();
}

void DirectXWaitQueueIdle(const Renderer* const pRenderer, Queue* pQueue)
{
	DIRECTX_ERROR_CHECK(pQueue->dx.queue->Signal(pQueue->dx.fence.dx.fence, pQueue->dx.fence.dx.fenceValue));
	DirectXWaitForFence(pRenderer, &pQueue->dx.fence);
}

D3D12_RESOURCE_STATES DirectXGetResourecState(const ResourceState state)
{
	D3D12_RESOURCE_STATES ret = D3D12_RESOURCE_STATE_COMMON;

	// These states cannot be combined with other states so we just do an == check
	if (state == RESOURCE_STATE_GENERIC_READ)
		return D3D12_RESOURCE_STATE_GENERIC_READ;
	if (state == RESOURCE_STATE_COMMON)
		return D3D12_RESOURCE_STATE_COMMON;
	if (state == RESOURCE_STATE_PRESENT)
		return D3D12_RESOURCE_STATE_PRESENT;

	if (state & RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER)
		ret |= D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
	if (state & RESOURCE_STATE_INDEX_BUFFER)
		ret |= D3D12_RESOURCE_STATE_INDEX_BUFFER;
	if (state & RESOURCE_STATE_RENDER_TARGET)
		ret |= D3D12_RESOURCE_STATE_RENDER_TARGET;
	if (state & RESOURCE_STATE_UNORDERED_ACCESS)
		ret |= D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	if (state & RESOURCE_STATE_DEPTH_WRITE)
		ret |= D3D12_RESOURCE_STATE_DEPTH_WRITE;
	else if (state & RESOURCE_STATE_DEPTH_READ)
		ret |= D3D12_RESOURCE_STATE_DEPTH_READ;
	if (state & RESOURCE_STATE_STREAM_OUT)
		ret |= D3D12_RESOURCE_STATE_STREAM_OUT;
	if (state & RESOURCE_STATE_INDIRECT_ARGUMENT)
		ret |= D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT;
	if (state & RESOURCE_STATE_COPY_DEST)
		ret |= D3D12_RESOURCE_STATE_COPY_DEST;
	if (state & RESOURCE_STATE_COPY_SOURCE)
		ret |= D3D12_RESOURCE_STATE_COPY_SOURCE;
	if (state & RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE)
		ret |= D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
	if (state & RESOURCE_STATE_PIXEL_SHADER_RESOURCE)
		ret |= D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

	return ret;
}


void DirectXResourceBarrier(const CommandBuffer* const pCommandBuffer, const uint32_t numBarrierInfos, const BarrierInfo* const pBarrierInfos)
{
	D3D12_RESOURCE_BARRIER barriers[MAX_NUM_BARRIERS]{};

	for (uint32_t i = 0; i < numBarrierInfos; ++i)
	{
		if (pBarrierInfos[i].currentState == RESOURCE_STATE_UNORDERED_ACCESS && pBarrierInfos[i].newState == RESOURCE_STATE_UNORDERED_ACCESS)
		{
			barriers[i].Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
			barriers[i].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;

			switch (pBarrierInfos[i].type)
			{
			case BARRIER_TYPE_BUFFER:
				barriers[i].UAV.pResource = pBarrierInfos[i].pBuffer->dx.resource;
				break;

			case BARRIER_TYPE_TEXTURE:
				barriers[i].UAV.pResource = pBarrierInfos[i].pTexture->dx.resource;
				break;

			case BARRIER_TYPE_RENDER_TARGET:
				barriers[i].UAV.pResource = pBarrierInfos[i].pRenderTarget->texture.dx.resource;
				break;
			}
		}
		else
		{
			barriers[i].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			barriers[i].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			barriers[i].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
			barriers[i].Transition.StateBefore = DirectXGetResourecState(pBarrierInfos[i].currentState);
			barriers[i].Transition.StateAfter = DirectXGetResourecState(pBarrierInfos[i].newState);

			switch (pBarrierInfos[i].type)
			{
			case BARRIER_TYPE_BUFFER:
				barriers[i].Transition.pResource = pBarrierInfos[i].pBuffer->dx.resource;
				break;

			case BARRIER_TYPE_TEXTURE:
				barriers[i].Transition.pResource = pBarrierInfos[i].pTexture->dx.resource;
				break;

			case BARRIER_TYPE_RENDER_TARGET:
				barriers[i].Transition.pResource = pBarrierInfos[i].pRenderTarget->texture.dx.resource;
				break;
			}
		}
	}

	pCommandBuffer->dx.commandList->ResourceBarrier(numBarrierInfos, barriers);
}

void DirectXInitialTransition(const Renderer* const pRenderer, const BarrierInfo* const pBarrierInfo)
{
	DirectXResetCommandBuffer(pRenderer, &pRenderer->commandBuffer);

	DirectXBeginCommandBuffer(&pRenderer->commandBuffer);

	DirectXResourceBarrier(&pRenderer->commandBuffer, 1, pBarrierInfo);

	DirectXEndCommandBuffer(&pRenderer->commandBuffer);

	QueueSubmitInfo submitInfo{};
	submitInfo.pCommandBuffer = &pRenderer->commandBuffer;
	submitInfo.pFence = &pRenderer->commandBuffer.fence;
	submitInfo.numSignalSemaphores = 0;
	submitInfo.signalSemaphores = nullptr;
	submitInfo.numWaitSemaphores = 0;
	submitInfo.waitSemaphores = nullptr;
	submitInfo.pQueue = &pRenderer->queue;
	DirectXQueueSubmit(&submitInfo);

	Queue queue = pRenderer->queue;
	DirectXWaitQueueIdle(pRenderer, &queue);
}

bool DirectXCheckTearingSupport(const Renderer* const pRenderer)
{
	bool allowTearing = false;

	IDXGIFactory4* factory4;
	HRESULT hr = CreateDXGIFactory2_d3d12_dll(0, IID_PPV_ARGS(&factory4));
	if (SUCCEEDED(hr))
	{
		IDXGIFactory5 *factory5;
		hr = factory4->QueryInterface(IID_PPV_ARGS(&factory5));
		if (SUCCEEDED(hr))
		{
			hr = factory5->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &allowTearing, sizeof(allowTearing));
		}
		SAFE_RELEASE(factory5);
	}
	SAFE_RELEASE(factory4);
	allowTearing = SUCCEEDED(hr) && allowTearing;

	return allowTearing;
}

void DirectXCreateRenderTarget(const Renderer* const pRenderer, const RenderTargetInfo* const pInfo, RenderTarget* pRenderTarget)
{
	bool const isDepth = TinyImageFormat_IsDepthOnly(pInfo->format) || TinyImageFormat_IsDepthAndStencil(pInfo->format);

	TextureInfo texInfo{};
	texInfo.filename = nullptr;
	texInfo.width = pInfo->width;
	texInfo.height = pInfo->height;
	texInfo.depth = 1;
	texInfo.arraySize = 1;
	texInfo.mipCount = 1;
	texInfo.format = pInfo->format;
	texInfo.dimension = TEXTURE_DIMENSION_2D;
	texInfo.type = pInfo->type;
	texInfo.isCubeMap = false;
	texInfo.isRenderTarget = true;
	texInfo.initialState = pInfo->initialState;
	texInfo.clearValue = pInfo->clearValue;

	CreateTexture(pRenderer, &texInfo, &pRenderTarget->texture);

	if (isDepth)
	{
		D3D12_DEPTH_STENCIL_VIEW_DESC viewDesc{};
		viewDesc.Format = (DXGI_FORMAT)TinyImageFormat_ToDXGI_FORMAT(pInfo->format);
		viewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		viewDesc.Flags = D3D12_DSV_FLAG_NONE;
		viewDesc.Texture2D.MipSlice = 0;

		DirectXDescriptroHeapAllocateInfo heapAllocateInfo{};
		heapAllocateInfo.type = VIEW_TYPE_DSV;
		heapAllocateInfo.pDsvDesc = &viewDesc;
		heapAllocateInfo.pRenderTarget = pRenderTarget;
		heapAllocateInfo.allocateOnGpuHeap = false;
		DirectXDescriptorHeapAllocate(pRenderer, &heapAllocateInfo, &gDsvHeap, nullptr);
	}
	else
	{
		D3D12_RENDER_TARGET_VIEW_DESC viewDesc{};
		viewDesc.Format = (DXGI_FORMAT)TinyImageFormat_ToDXGI_FORMAT(pInfo->format);
		viewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		viewDesc.Texture2D.MipSlice = 0;
		viewDesc.Texture2D.PlaneSlice = 0;

		DirectXDescriptroHeapAllocateInfo heapAllocateInfo{};
		heapAllocateInfo.type = VIEW_TYPE_RTV;
		heapAllocateInfo.pRtvDesc = &viewDesc;
		heapAllocateInfo.pRenderTarget = pRenderTarget;
		heapAllocateInfo.allocateOnGpuHeap = false;
		DirectXDescriptorHeapAllocate(pRenderer, &heapAllocateInfo, &gRtvHeap, nullptr);
	}

	pRenderTarget->info = *pInfo;
}

void DirectXDestroyRenderTarget(const Renderer* const pRenderer, RenderTarget* pRenderTarget)
{
	DirectXDescriptorHeapFree(&gRtvHeap, pRenderTarget->dx.descriptorId, 0);
	SAFE_RELEASE(pRenderTarget->texture.dx.resource);
	SAFE_RELEASE(pRenderTarget->texture.dx.allocation);
}

void DirectXCreateSwapChain(const Renderer* const pRenderer, const SwapChainInfo* const pInfo, SwapChain* pSwapChain)
{
	uint32_t bufferCount = 3;

	pSwapChain->numRenderTargets = bufferCount;

	DXGI_SWAP_CHAIN_DESC1 desc{};
	desc.Width = pInfo->width;
	desc.Height = pInfo->height;
	desc.Format = (DXGI_FORMAT)TinyImageFormat_ToDXGI_FORMAT(pInfo->format);
	desc.Stereo = false;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.BufferCount = bufferCount;
	desc.Scaling = DXGI_SCALING_STRETCH;
	desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	desc.Flags = DirectXCheckTearingSupport(pRenderer) ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;

	pSwapChain->dx.flags = desc.Flags;

	IDXGISwapChain1* swapChain1 = nullptr;
	DIRECTX_ERROR_CHECK(pRenderer->dx.factory->CreateSwapChainForHwnd(pInfo->queue->dx.queue, pInfo->window->wndHandle, &desc,
		nullptr, nullptr, &swapChain1));

	// Disable the Alt+Enter fullscreen toggle feature. Switching to fullscreen
	// will be handled manually.
	DIRECTX_ERROR_CHECK(pRenderer->dx.factory->MakeWindowAssociation(pInfo->window->wndHandle, DXGI_MWA_NO_ALT_ENTER));

	DIRECTX_ERROR_CHECK(swapChain1->QueryInterface(IID_PPV_ARGS(&pSwapChain->dx.swapChain)));

	SAFE_RELEASE(swapChain1);

	pSwapChain->pRenderTargets = (RenderTarget*)calloc(bufferCount, sizeof(RenderTarget));
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = gRtvHeap.cpuHeap->GetCPUDescriptorHandleForHeapStart();

	for (uint32_t i = 0; i < bufferCount; ++i)
	{
		DIRECTX_ERROR_CHECK(pSwapChain->dx.swapChain->GetBuffer(i, IID_PPV_ARGS(&pSwapChain->pRenderTargets[i].texture.dx.resource)));

		DirectXDescriptroHeapAllocateInfo heapAllocateInfo{};
		heapAllocateInfo.type = VIEW_TYPE_RTV;
		heapAllocateInfo.pRtvDesc = nullptr;
		heapAllocateInfo.pRenderTarget = &pSwapChain->pRenderTargets[i];
		heapAllocateInfo.allocateOnGpuHeap = false;
		DirectXDescriptorHeapAllocate(pRenderer, &heapAllocateInfo, &gRtvHeap, nullptr);

		pSwapChain->pRenderTargets[i].info.width = pInfo->width;
		pSwapChain->pRenderTargets[i].info.height = pInfo->height;
		pSwapChain->pRenderTargets[i].info.format = pInfo->format;
		pSwapChain->pRenderTargets[i].info.clearValue = pInfo->clearValue;
	}

	pSwapChain->info = *pInfo;
}

void DirectXDestroySwapChain(const Renderer* const pRenderer, SwapChain* pSwapChain)
{
	for (uint32_t i = 0; i < pSwapChain->numRenderTargets; ++i)
	{
		DirectXDescriptorHeapFree(&gRtvHeap, pSwapChain->pRenderTargets[i].dx.descriptorId, 0);
		SAFE_RELEASE(pSwapChain->pRenderTargets[i].texture.dx.resource);
	}
	SAFE_FREE(pSwapChain->pRenderTargets);
	SAFE_RELEASE(pSwapChain->dx.swapChain);
}

void DirectXCreateShader(const Renderer* const pRenderer, const ShaderInfo* const pInfo, Shader* pShader)
{
	char currentDirectory[MAX_FILE_PATH]{};
	GetCurrentPath(currentDirectory);

	char inputFileWithPath[MAX_FILE_PATH]{};
	strcat_s(inputFileWithPath, currentDirectory);
	strcat_s(inputFileWithPath, "CompiledShaders\\HLSL\\");
	strcat_s(inputFileWithPath, pInfo->filename);

	char* buffer = nullptr;
	uint32_t fileSize = 0;
	ReadFile(inputFileWithPath, &buffer, &fileSize, BINARY);

	pShader->dx.shader.pShaderBytecode = buffer;
	pShader->dx.shader.BytecodeLength = fileSize;
}

void DirectXDestroyShader(const Renderer* const pRenderer, Shader* pShader)
{
	SAFE_FREE(pShader->dx.shader.pShaderBytecode);
}

void DirectXCreateRootSignature(const Renderer* const pRenderer, const RootSignatureInfo* const pInfo, RootSignature* pRootSignature)
{
	pRootSignature->dx.rootParamterIndices[UPDATE_FREQUENCY_PER_NONE] = -1;
	pRootSignature->dx.rootParamterIndices[UPDATE_FREQUENCY_PER_DRAW] = -1;
	pRootSignature->dx.rootParamterIndices[UPDATE_FREQUENCY_PER_FRAME] = -1;
	pRootSignature->dx.rootParameterSamplerIndex = -1;
	pRootSignature->dx.rootConstantsIndex = -1;

	D3D12_DESCRIPTOR_RANGE1* perNoneRanges = nullptr;
	D3D12_DESCRIPTOR_RANGE1* perDrawRanges = nullptr;
	D3D12_DESCRIPTOR_RANGE1* perFrameRanges = nullptr;
	D3D12_DESCRIPTOR_RANGE1* samplerRanges = nullptr;

	for (uint32_t i = 0; i < pInfo->numRootParameterInfos; ++i)
	{
		D3D12_DESCRIPTOR_RANGE_TYPE type{};
		switch (pInfo->pRootParameterInfos[i].type)
		{
		case DESCRIPTOR_TYPE_UNIFORM_BUFFER:
			type = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
			break;

		case DESCRIPTOR_TYPE_BUFFER:
		case DESCRIPTOR_TYPE_TEXTURE:
			type = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
			break;

		case DESCRIPTOR_TYPE_RW_BUFFER:
		case DESCRIPTOR_TYPE_RW_TEXTURE:
			type = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
			break;
		}

		if (pInfo->pRootParameterInfos[i].type != DESCRIPTOR_TYPE_SAMPLER)
		{
			if (pInfo->pRootParameterInfos[i].updateFrequency == UPDATE_FREQUENCY_PER_NONE)
			{
				D3D12_DESCRIPTOR_RANGE1 perNone{};
			
				perNone.RangeType = type;
				perNone.NumDescriptors = pInfo->pRootParameterInfos[i].numDescriptors;
				perNone.BaseShaderRegister = pInfo->pRootParameterInfos[i].baseRegister;
				perNone.RegisterSpace = pInfo->pRootParameterInfos[i].registerSpace;
				perNone.Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE;
				perNone.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

				arrpush(perNoneRanges, perNone);
			}
			else if (pInfo->pRootParameterInfos[i].updateFrequency == UPDATE_FREQUENCY_PER_DRAW)
			{
				D3D12_DESCRIPTOR_RANGE1 perDraw{};

				perDraw.RangeType = type;
				perDraw.NumDescriptors = pInfo->pRootParameterInfos[i].numDescriptors;
				perDraw.BaseShaderRegister = pInfo->pRootParameterInfos[i].baseRegister;
				perDraw.RegisterSpace = pInfo->pRootParameterInfos[i].registerSpace;
				perDraw.Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE;
				perDraw.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

				arrpush(perDrawRanges, perDraw);
			}
			else// UPDATE_FREQUENCY_PER_FRAME
			{
				D3D12_DESCRIPTOR_RANGE1 perFrame{};

				perFrame.RangeType = type;
				perFrame.NumDescriptors = pInfo->pRootParameterInfos[i].numDescriptors;
				perFrame.BaseShaderRegister = pInfo->pRootParameterInfos[i].baseRegister;
				perFrame.RegisterSpace = pInfo->pRootParameterInfos[i].registerSpace;
				perFrame.Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE;
				perFrame.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

				arrpush(perFrameRanges, perFrame);
			}
		}
		else //type == DESCRIPTOR_TYPE_SAMPLER
		{
			D3D12_DESCRIPTOR_RANGE1 samplerRange{};
			samplerRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
			samplerRange.NumDescriptors = pInfo->pRootParameterInfos[i].numDescriptors;
			samplerRange.BaseShaderRegister = pInfo->pRootParameterInfos[i].baseRegister;
			samplerRange.RegisterSpace = pInfo->pRootParameterInfos[i].registerSpace;
			samplerRange.Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE;
			samplerRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

			arrpush(samplerRanges, samplerRange);
		}
	}

	D3D12_ROOT_PARAMETER1* rootParameters = nullptr;
	D3D12_ROOT_PARAMETER1 rootParameter{};

	D3D12_ROOT_DESCRIPTOR_TABLE1 dTable{};

	if (arrlenu(perNoneRanges) > 0)
	{
		dTable.NumDescriptorRanges = arrlenu(perNoneRanges);
		dTable.pDescriptorRanges = perNoneRanges;

		rootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		rootParameter.DescriptorTable = dTable;
		rootParameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

		pRootSignature->dx.rootParamterIndices[UPDATE_FREQUENCY_PER_NONE] = arrlenu(rootParameters);
		arrpush(rootParameters, rootParameter);
	}
	if (arrlenu(perDrawRanges) > 0)
	{
		dTable.NumDescriptorRanges = arrlenu(perDrawRanges);
		dTable.pDescriptorRanges = perDrawRanges;

		rootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		rootParameter.DescriptorTable = dTable;
		rootParameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

		pRootSignature->dx.rootParamterIndices[UPDATE_FREQUENCY_PER_DRAW] = arrlenu(rootParameters);
		arrpush(rootParameters, rootParameter);
	}

	if (arrlenu(perFrameRanges) > 0)
	{
		dTable.NumDescriptorRanges = arrlenu(perFrameRanges);
		dTable.pDescriptorRanges = perFrameRanges;

		rootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		rootParameter.DescriptorTable = dTable;
		rootParameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

		pRootSignature->dx.rootParamterIndices[UPDATE_FREQUENCY_PER_FRAME] = arrlenu(rootParameters);
		arrpush(rootParameters, rootParameter);
	}

	if (arrlenu(samplerRanges) > 0)
	{
		dTable.NumDescriptorRanges = arrlenu(samplerRanges);
		dTable.pDescriptorRanges = samplerRanges;

		rootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		rootParameter.DescriptorTable = dTable;
		rootParameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

		pRootSignature->dx.rootParameterSamplerIndex = arrlenu(rootParameters);
		arrpush(rootParameters, rootParameter);
	}

	if (pInfo->useRootConstants)
	{
		D3D12_ROOT_CONSTANTS constants{};
		constants.Num32BitValues = pInfo->rootConstantsInfo.numValues;
		constants.RegisterSpace = pInfo->rootConstantsInfo.registerSpace;
		constants.ShaderRegister = pInfo->rootConstantsInfo.baseRegister;

		rootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
		rootParameter.Constants = constants;
		rootParameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

		pRootSignature->dx.rootConstantsIndex = arrlenu(rootParameters);
		arrpush(rootParameters, rootParameter);
	}

	D3D12_ROOT_SIGNATURE_DESC1 rootSigDesc{};
	rootSigDesc.NumParameters = arrlenu(rootParameters);
	rootSigDesc.pParameters = rootParameters;
	rootSigDesc.NumStaticSamplers = 0;
	rootSigDesc.pStaticSamplers = nullptr;

	rootSigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

	if(pInfo->useInputLayout)
		rootSigDesc.Flags |= D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	D3D12_VERSIONED_ROOT_SIGNATURE_DESC versionRootSigDesc{};
	versionRootSigDesc.Desc_1_1 = rootSigDesc;
	versionRootSigDesc.Version = D3D_ROOT_SIGNATURE_VERSION_1_1;

	ID3DBlob* serializedRootSig = nullptr;
	ID3DBlob* errorBlob = nullptr;

	DIRECTX_ERROR_CHECK(SerializeVersionedRootSignature_d3d12_dll(&versionRootSigDesc, &serializedRootSig, &errorBlob));
	if (errorBlob != nullptr)
	{
		OutputDebugStringA((const char*)errorBlob->GetBufferPointer());
	}

	DIRECTX_ERROR_CHECK(pRenderer->dx.device->CreateRootSignature(0, serializedRootSig->GetBufferPointer(), serializedRootSig->GetBufferSize(),
		IID_PPV_ARGS(&pRootSignature->dx.rootSignature)));

	SAFE_RELEASE(serializedRootSig);
	SAFE_RELEASE(errorBlob);
	
	arrfree(rootParameters);
	arrfree(perNoneRanges);
	arrfree(perDrawRanges);
	arrfree(perFrameRanges);
	arrfree(samplerRanges);
}

void DirectXDestroyRootSignature(const Renderer* const pRenderer, RootSignature* pRootSignature)
{
	SAFE_RELEASE(pRootSignature->dx.rootSignature);
}

D3D12_BLEND DirectXBlendFactorToD3D12Blend(const BlendFactor factor)
{
	switch (factor)
	{
	case BLEND_FACTOR_ZERO:
		return D3D12_BLEND_ZERO;

	case BLEND_FACTOR_ONE:
		return D3D12_BLEND_ONE;

	case BLEND_FACTOR_SRC_COLOR:
		return D3D12_BLEND_SRC_COLOR;

	case BLEND_FACTOR_ONE_MINUS_SRC_COLOR:
		return D3D12_BLEND_INV_SRC_COLOR;

	case BLEND_FACTOR_DST_COLOR:
		return D3D12_BLEND_DEST_COLOR;

	case BLEND_FACTOR_ONE_MINUS_DST_COLOR:
		return D3D12_BLEND_INV_DEST_COLOR;

	case BLEND_FACTOR_SRC_ALPHA:
		return D3D12_BLEND_SRC_ALPHA;

	case BLEND_FACTOR_ONE_MINUS_SRC_ALPHA:
		return D3D12_BLEND_INV_SRC_ALPHA;

	case BLEND_FACTOR_DST_ALPHA:
		return D3D12_BLEND_DEST_ALPHA;

	case BLEND_FACTOR_ONE_MINUS_DST_ALPHA:
		return D3D12_BLEND_INV_DEST_ALPHA;
	}

	return D3D12_BLEND_ONE;
}

void DirectXCreateBlendDesc(const PipelineInfo* const pInfo, D3D12_RENDER_TARGET_BLEND_DESC* pBlendDesc)
{
	pBlendDesc->BlendEnable = pInfo->blendInfo.enableBlend;
	pBlendDesc->LogicOpEnable = false;
	pBlendDesc->SrcBlend = DirectXBlendFactorToD3D12Blend(pInfo->blendInfo.srcColorBlendFactor);
	pBlendDesc->DestBlend = DirectXBlendFactorToD3D12Blend(pInfo->blendInfo.dstColorBlendFactor);
	pBlendDesc->BlendOp = (D3D12_BLEND_OP)(pInfo->blendInfo.colorBlendOp + 1);
	pBlendDesc->SrcBlendAlpha = DirectXBlendFactorToD3D12Blend(pInfo->blendInfo.srcAlphaBlendFactor);
	pBlendDesc->DestBlendAlpha = DirectXBlendFactorToD3D12Blend(pInfo->blendInfo.dstAlphaBlendFactor);
	pBlendDesc->BlendOpAlpha = (D3D12_BLEND_OP)(pInfo->blendInfo.alphaBlendOp + 1);
	pBlendDesc->LogicOp = D3D12_LOGIC_OP_CLEAR;
	pBlendDesc->RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
}

void DirectXCreateRasterizerDesc(const PipelineInfo* const pInfo, D3D12_RASTERIZER_DESC* pRasterDesc)
{
	switch (pInfo->rasInfo.fillMode)
	{
	case FILL_MODE_SOLID:
		pRasterDesc->FillMode = D3D12_FILL_MODE_SOLID;
		break;

	case FILL_MODE_WIREFRAME:
		pRasterDesc->FillMode = D3D12_FILL_MODE_WIREFRAME;
		break;
	}

	switch (pInfo->rasInfo.cullMode)
	{
	case CULL_MODE_NONE:
		pRasterDesc->CullMode = D3D12_CULL_MODE_NONE;
		break;

	case CULL_MODE_FRONT:
		pRasterDesc->CullMode = D3D12_CULL_MODE_FRONT;
		break;

	case CULL_MODE_BACK:
		pRasterDesc->CullMode = D3D12_CULL_MODE_BACK;
		break;
	}

	switch (pInfo->rasInfo.faceMode)
	{
	case FACE_CLOCKWISE:
		pRasterDesc->FrontCounterClockwise = false;
		break;

	case FACE_COUNTER_CLOCKWISE:
		pRasterDesc->FrontCounterClockwise = true;
		break;
	}

	pRasterDesc->DepthBias = 0;
	pRasterDesc->DepthBiasClamp = 0.0f;
	pRasterDesc->SlopeScaledDepthBias = 0.0f;
	pRasterDesc->DepthClipEnable = true;
	pRasterDesc->MultisampleEnable = false;
	pRasterDesc->AntialiasedLineEnable = false;
	pRasterDesc->ForcedSampleCount = 0;
	pRasterDesc->ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
}

D3D12_COMPARISON_FUNC GetDepthFunction(DepthFunction func)
{
	switch (func)
	{
	case DEPTH_FUNCTION_NONE:
		return D3D12_COMPARISON_FUNC_NONE;

	case DEPTH_FUNCTION_NEVER:
		return D3D12_COMPARISON_FUNC_NEVER;

	case DEPTH_FUNCTION_LESS:
		return D3D12_COMPARISON_FUNC_LESS;

	case DEPTH_FUNCTION_EQUAL:
		return D3D12_COMPARISON_FUNC_EQUAL;

	case DEPTH_FUNCTION_LESS_OR_EQUAL:
		return D3D12_COMPARISON_FUNC_LESS_EQUAL;

	case DEPTH_FUNCTION_GREATER:
		return D3D12_COMPARISON_FUNC_GREATER;

	case DEPTH_FUNCTION_NOT_EQUAL:
		return D3D12_COMPARISON_FUNC_NOT_EQUAL;

	case DEPTH_FUNCTION_GREATER_OR_EQUAL:
		return D3D12_COMPARISON_FUNC_GREATER_EQUAL;

	case DEPTH_FUNCTION_ALWAYS:
		return D3D12_COMPARISON_FUNC_ALWAYS;
	}

	return D3D12_COMPARISON_FUNC_NONE;
}

void DirectXCreateDepthStencilDesc(const PipelineInfo* const pInfo, D3D12_DEPTH_STENCIL_DESC* pDesc)
{
	pDesc->DepthEnable = pInfo->depthInfo.depthTestEnable;

	if (pInfo->depthInfo.depthWriteEnable)
		pDesc->DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	else
		pDesc->DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;

	pDesc->DepthFunc = GetDepthFunction(pInfo->depthInfo.depthFunction);

	pDesc->StencilEnable = false;
	pDesc->StencilReadMask = 0xff;
	pDesc->StencilWriteMask = 0xff;
	pDesc->FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	pDesc->FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	pDesc->FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	pDesc->FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	pDesc->BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	pDesc->BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	pDesc->BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	pDesc->BackFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
}

void DirectXCreateInputLayoutDesc(const PipelineInfo* const pInfo, 
	D3D12_INPUT_ELEMENT_DESC* pElementDescs, D3D12_INPUT_LAYOUT_DESC* pLayoutDesc)
{
	for (uint32_t i = 0; i < pInfo->pVertexInputInfo->numVertexAttributes; ++i)
	{
		pElementDescs[i].SemanticName = pInfo->pVertexInputInfo->vertexAttributes[i].semanticName;
		pElementDescs[i].SemanticIndex = pInfo->pVertexInputInfo->vertexAttributes[i].semanticIndex;
		pElementDescs[i].Format = (DXGI_FORMAT)TinyImageFormat_ToDXGI_FORMAT(pInfo->pVertexInputInfo->vertexAttributes[i].format);
		pElementDescs[i].InputSlot = 0;
		pElementDescs[i].AlignedByteOffset = pInfo->pVertexInputInfo->vertexAttributes[i].offset;

		switch (pInfo->pVertexInputInfo->vertexBinding.inputRate)
		{
		case INPUT_RATE_PER_VERTEX:
			pElementDescs[i].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
			break;

		case INPUT_RATE_PER_INSTANCE:
			pElementDescs[i].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA;
			break;
		}
		pElementDescs[i].InstanceDataStepRate = 0;
	}

	pLayoutDesc->pInputElementDescs = pElementDescs;
	pLayoutDesc->NumElements = pInfo->pVertexInputInfo->numVertexAttributes;
}

D3D12_PRIMITIVE_TOPOLOGY TranslateTopologyToD3D12Topology(Topology topology)
{
	switch (topology)
	{
	case TOPOLOGY_POINT_LIST:
		return D3D_PRIMITIVE_TOPOLOGY_POINTLIST;

	case TOPOLOGY_LINE_LIST:
		return D3D_PRIMITIVE_TOPOLOGY_LINELIST;

	case TOPOLOGY_TRIANGLE_LIST:
		return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	}

	return D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
}

D3D12_PRIMITIVE_TOPOLOGY_TYPE TranslateTopologyToD3D12PrimitiveTopology(Topology topology)
{
	switch (topology)
	{
	case TOPOLOGY_POINT_LIST:
		return D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;

	case TOPOLOGY_LINE_LIST:
		return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;

	case TOPOLOGY_TRIANGLE_LIST:
		return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	}

	return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
}


void DirectXCreateGraphicsPipleine(const Renderer* const pRenderer, const PipelineInfo* const pInfo, Pipeline* pPipeline)
{
	D3D12_RENDER_TARGET_BLEND_DESC blendDesc{};
	DirectXCreateBlendDesc(pInfo, &blendDesc);

	D3D12_RASTERIZER_DESC rasterDesc{};
	DirectXCreateRasterizerDesc(pInfo, &rasterDesc);

	D3D12_DEPTH_STENCIL_DESC dsDesc{};
	DirectXCreateDepthStencilDesc(pInfo, &dsDesc);

	D3D12_INPUT_ELEMENT_DESC elementDescs[MAX_NUM_VERTEX_ATTRIBUTES]{};
	D3D12_INPUT_LAYOUT_DESC layoutDesc{};
	DirectXCreateInputLayoutDesc(pInfo, elementDescs, &layoutDesc);

	D3D12_GRAPHICS_PIPELINE_STATE_DESC desc{};
	ZeroMemory(&desc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	desc.pRootSignature = pInfo->pRootSignature->dx.rootSignature;
	desc.VS = pInfo->pVertexShader->dx.shader;
	desc.PS = pInfo->pPixelShader->dx.shader;
	desc.StreamOutput.pSODeclaration = nullptr;
	desc.StreamOutput.pBufferStrides = nullptr;
	desc.StreamOutput.NumEntries = 0;
	desc.StreamOutput.NumStrides = 0;
	desc.StreamOutput.RasterizedStream = 0;
	desc.BlendState.AlphaToCoverageEnable = false;
	desc.BlendState.IndependentBlendEnable = false;
	desc.SampleMask = UINT_MAX;
	desc.RasterizerState = rasterDesc;
	desc.DepthStencilState = dsDesc;
	desc.InputLayout = layoutDesc;
	desc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
	desc.PrimitiveTopologyType = TranslateTopologyToD3D12PrimitiveTopology(pInfo->topology);
	desc.NumRenderTargets = pInfo->numRenderTargets;

	for (uint32_t i = 0; i < pInfo->numRenderTargets; ++i)
	{
		desc.RTVFormats[i] = (DXGI_FORMAT)TinyImageFormat_ToDXGI_FORMAT(pInfo->renderTargetFormat[i]);
		desc.BlendState.RenderTarget[i] = blendDesc;
	}

	desc.DSVFormat = (DXGI_FORMAT)TinyImageFormat_ToDXGI_FORMAT(pInfo->depthFormat);
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.NodeMask = 0;
	desc.CachedPSO.pCachedBlob = nullptr;
	desc.CachedPSO.CachedBlobSizeInBytes = 0;
	desc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	DIRECTX_ERROR_CHECK(pRenderer->dx.device->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&pPipeline->dx.pipeline)));
}

void DirectXCreateComputePipleine(const Renderer* const pRenderer, const PipelineInfo* const pInfo, Pipeline* pPipeline)
{
	D3D12_COMPUTE_PIPELINE_STATE_DESC desc{};
	desc.pRootSignature = pInfo->pRootSignature->dx.rootSignature;
	desc.CS = pInfo->pComputeShader->dx.shader;
	desc.NodeMask = 0;
	desc.CachedPSO.CachedBlobSizeInBytes = 0;
	desc.CachedPSO.pCachedBlob = nullptr;
	desc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	DIRECTX_ERROR_CHECK(pRenderer->dx.device->CreateComputePipelineState(&desc, IID_PPV_ARGS(&pPipeline->dx.pipeline)));
}

void DirectXCreatePipeline(const Renderer* const pRenderer, const PipelineInfo* const pInfo, Pipeline* pPipeline)
{
	switch (pInfo->type)
	{
	case PIPELINE_TYPE_GRAPHICS:
		DirectXCreateGraphicsPipleine(pRenderer, pInfo, pPipeline);
		break;

	case PIPELINE_TYPE_COMPUTE:
		DirectXCreateComputePipleine(pRenderer, pInfo, pPipeline);
		break;
	}

	pPipeline->pRootSignature = pInfo->pRootSignature;
	pPipeline->pRootSignature->pipelineType = pInfo->type;
	pPipeline->topology = pInfo->topology;
	pPipeline->type = pInfo->type;
}

void DirectXDestroyPipeline(const Renderer* const pRenderer, Pipeline* pPipeline)
{
	SAFE_RELEASE(pPipeline->dx.pipeline);
}

void DirectXAcquireNextImage(const Renderer* const pRenderer, const SwapChain* const pSwapChain, 
	const Semaphore* const pSemaphore, uint32_t* pImageIndex)
{
	*pImageIndex = pSwapChain->dx.swapChain->GetCurrentBackBufferIndex();
}

void DirectXBindRenderTarget(CommandBuffer* pCommandBuffer, const BindRenderTargetInfo* const pInfo)
{
	if (pInfo == nullptr)
		return;

	bool hasRenderTarget = false;
	bool hasDepthStencil = false;

	if (pInfo->pRenderTarget != nullptr)
	{
		hasRenderTarget = true;
	}

	if (pInfo->pDepthTarget != nullptr)
	{
		hasDepthStencil = true;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle(gRtvHeap.cpuHeap->GetCPUDescriptorHandleForHeapStart());
	if (hasRenderTarget)
	{
		rtvHandle.ptr += (pInfo->pRenderTarget->dx.descriptorId * gRtvHeap.descriptorSize);
		if (pInfo->renderTargetLoadOp == LOAD_OP_CLEAR)
		{
			const float clearColor[] = { pInfo->pRenderTarget->info.clearValue.r, pInfo->pRenderTarget->info.clearValue.g,
				pInfo->pRenderTarget->info.clearValue.b, pInfo->pRenderTarget->info.clearValue.a };
			pCommandBuffer->dx.commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
		}
	}
	
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle(gDsvHeap.cpuHeap->GetCPUDescriptorHandleForHeapStart());
	if (hasDepthStencil)
	{
		dsvHandle.ptr += (pInfo->pDepthTarget->dx.descriptorId * gDsvHeap.descriptorSize);
		if (pInfo->depthTargetLoadOp == LOAD_OP_CLEAR)
		{
			pCommandBuffer->dx.commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, pInfo->pDepthTarget->info.clearValue.depth,
				pInfo->pDepthTarget->info.clearValue.stencil, 0, nullptr);
		}
	}

	if (hasRenderTarget == false && hasDepthStencil == false)
	{
		pCommandBuffer->dx.commandList->OMSetRenderTargets(0, nullptr, false, nullptr);
	}
	else if (hasRenderTarget == true && hasDepthStencil == false)
	{
		pCommandBuffer->dx.commandList->OMSetRenderTargets(1, &rtvHandle, false, nullptr);
	}
	else if (hasRenderTarget == false && hasDepthStencil == true)
	{
		pCommandBuffer->dx.commandList->OMSetRenderTargets(0, nullptr, false, &dsvHandle);
	}
	else //hasRenderTarget == true && hasDepthStencil == true
	{
		pCommandBuffer->dx.commandList->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);
	}
}

void DirectXSetViewport(const CommandBuffer* const pCommandBuffer, const ViewportInfo* const viewportInfo)
{
	D3D12_VIEWPORT viewport{};
	viewport.TopLeftX = viewportInfo->x;
	viewport.TopLeftY = viewportInfo->y;
	viewport.Width = viewportInfo->width;
	viewport.Height = viewportInfo->height;
	viewport.MinDepth = viewportInfo->minDepth;
	viewport.MaxDepth = viewportInfo->maxDepth;
	pCommandBuffer->dx.commandList->RSSetViewports(1, &viewport);
}

void DirectXSetScissor(const CommandBuffer* const pCommandBuffer, const ScissorInfo* const scissorInfo)
{
	D3D12_RECT rect{};
	rect.left = scissorInfo->x;
	rect.right = scissorInfo->width;
	rect.top = scissorInfo->y;
	rect.bottom = scissorInfo->height;
	pCommandBuffer->dx.commandList->RSSetScissorRects(1, &rect);
}

void DirectXBindPipeline(CommandBuffer* pCommandBuffer, const Pipeline* const pPipeline)
{
	if (pPipeline->type == PIPELINE_TYPE_GRAPHICS)
	{
		pCommandBuffer->dx.commandList->SetGraphicsRootSignature(pPipeline->pRootSignature->dx.rootSignature);
		pCommandBuffer->dx.commandList->IASetPrimitiveTopology(TranslateTopologyToD3D12Topology(pPipeline->topology));
		pCommandBuffer->dx.commandList->SetPipelineState(pPipeline->dx.pipeline);

	}
	else // PIPELINE_TYPE_COMPUTE
	{
		pCommandBuffer->dx.commandList->SetComputeRootSignature(pPipeline->pRootSignature->dx.rootSignature);
		pCommandBuffer->dx.commandList->SetPipelineState(pPipeline->dx.pipeline);
	}

	pCommandBuffer->pCurrentPipeline = pPipeline;
}

void DirectXDraw(const CommandBuffer* const pCommandBuffer, const uint32_t vertexCount,
	const uint32_t instanceCount, const  uint32_t firstVertex, const uint32_t firstInstance)
{
	pCommandBuffer->dx.commandList->DrawInstanced(vertexCount, instanceCount, firstVertex, firstInstance);
}

void DirectXDrawIndexed(const CommandBuffer* const CommandBuffer, const uint32_t indexCount,
	const uint32_t instanceCount, const uint32_t firstIndex, const uint32_t vertexOffset, const uint32_t firstInstance)
{
	CommandBuffer->dx.commandList->DrawIndexedInstanced(indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}

void DirectXDispatch(const CommandBuffer* const pCommandBuffer, const uint32_t x, const uint32_t y, const uint32_t z)
{
	pCommandBuffer->dx.commandList->Dispatch(x, y, z);
}

void DirectXEndCommandList(const CommandBuffer* const pCommandBuffer)
{
	DIRECTX_ERROR_CHECK(pCommandBuffer->dx.commandList->Close());
}

void DirectXQueuePresent(const PresentInfo* const pInfo)
{
	pInfo->pSwapChain->dx.swapChain->Present(0, pInfo->pSwapChain->dx.flags);
}

void DirectXMapMemory(const Renderer* const pRenderer, const Buffer* const pBuffer, void** ppData)
{
	DIRECTX_ERROR_CHECK(pBuffer->dx.resource->Map(0, nullptr, ppData));
}

void DirectXUnmapMemory(const Renderer* const pRenderer, const Buffer* const pBuffer)
{
	pBuffer->dx.resource->Unmap(0, nullptr);
}

void DirectXCreateStagingBuffer(const Renderer* const pRenderer, const uint32_t numBytes)
{
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
	resourceDesc.Width = numBytes;
	resourceDesc.Height = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.SampleDesc.Quality = 0;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	D3D12MA::ALLOCATION_DESC allocationDesc{};
	allocationDesc.Flags = D3D12MA::ALLOCATION_FLAG_NONE;
	allocationDesc.HeapType = D3D12_HEAP_TYPE_UPLOAD;
	allocationDesc.ExtraHeapFlags = D3D12_HEAP_FLAG_NONE;
	allocationDesc.pPrivateData = nullptr;

	DIRECTX_ERROR_CHECK(pRenderer->dx.allocator->CreateResource(&allocationDesc, &resourceDesc,
		D3D12_RESOURCE_STATE_COMMON, nullptr, &gDirectXCopyEngine.stagingBuffer.dx.allocation,
		IID_PPV_ARGS(&gDirectXCopyEngine.stagingBuffer.dx.resource)));
}

void DirectXDestroyStagingBuffer(const Renderer* const pRenderer)
{
	SAFE_RELEASE(gDirectXCopyEngine.stagingBuffer.dx.resource);
	SAFE_RELEASE(gDirectXCopyEngine.stagingBuffer.dx.allocation);
}

void DirectXCopyBuffer(const Renderer* const pRenderer, const BufferInfo* const pInfo,
	const Buffer* const pDstBuffer, uint32_t numBytes)
{
	DirectXCreateStagingBuffer(pRenderer, numBytes);

	void* data = nullptr;
	DirectXMapMemory(pRenderer, &gDirectXCopyEngine.stagingBuffer, &data);
	memcpy(data, pInfo->data, pInfo->size);
	DirectXUnmapMemory(pRenderer, &gDirectXCopyEngine.stagingBuffer);

	DirectXBeginCopyEngineCommandList(pRenderer);

	BarrierInfo barrierInfo{};
	barrierInfo.type = BARRIER_TYPE_BUFFER;
	barrierInfo.pBuffer = &gDirectXCopyEngine.stagingBuffer;
	barrierInfo.currentState = RESOURCE_STATE_COMMON;
	barrierInfo.newState = RESOURCE_STATE_COPY_SOURCE;
	DirectXResourceBarrier(&gDirectXCopyEngine.copyCommandBuffer, 1, &barrierInfo);

	barrierInfo.type = BARRIER_TYPE_BUFFER;
	barrierInfo.pBuffer = pDstBuffer;
	barrierInfo.currentState = RESOURCE_STATE_COMMON;
	barrierInfo.newState = RESOURCE_STATE_COPY_DEST;
	DirectXResourceBarrier(&gDirectXCopyEngine.copyCommandBuffer, 1, &barrierInfo);

	gDirectXCopyEngine.copyCommandBuffer.dx.commandList->CopyBufferRegion(pDstBuffer->dx.resource, 0,
		gDirectXCopyEngine.stagingBuffer.dx.resource, 0, numBytes);

	DirectXEndCopyEngineCommandList();

	DirectXCopyEngineSubmit(pRenderer);

	BarrierInfo barrier{};
	barrier.type = BARRIER_TYPE_BUFFER;
	barrier.pBuffer = pDstBuffer;
	barrier.currentState = RESOURCE_STATE_COPY_DEST;
	barrier.newState = pInfo->initialState;
	DirectXInitialTransition(pRenderer, &barrier);

	DirectXDestroyStagingBuffer(pRenderer);
}

void DirectXCreateBuffer(const Renderer* const pRenderer, const BufferInfo* pInfo, Buffer* pBuffer)
{
	bool copyData = false;
	if (pInfo->data != nullptr && pInfo->usage == MEMORY_USAGE_GPU_ONLY)
		copyData = true;

	uint64_t allocationSize = pInfo->size;

	//Align the buffer size to a multiple of 256
	if (pInfo->type & BUFFER_TYPE_UNIFORM)
	{
		allocationSize = (allocationSize + 255) & ~255;
	}

	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
	resourceDesc.Width = allocationSize;
	resourceDesc.Height = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.SampleDesc.Quality = 0;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	if (pInfo->type & BUFFER_TYPE_RW_BUFFER)
	{
		resourceDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	}

	// Adjust for padding
	uint64_t padded_size = 0;
	pRenderer->dx.device->GetCopyableFootprints(&resourceDesc, 0, 1, 0, NULL, NULL, NULL, &padded_size);
	allocationSize = padded_size;
	resourceDesc.Width = allocationSize;

	D3D12MA::ALLOCATION_DESC allocationDesc{};
	allocationDesc.Flags = D3D12MA::ALLOCATION_FLAG_NONE;

	switch (pInfo->usage)
	{
	case MEMORY_USAGE_GPU_ONLY:
		allocationDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;
		break;

	case MEMORY_USAGE_CPU_ONLY:
	case MEMORY_USAGE_CPU_TO_GPU:
		allocationDesc.HeapType = D3D12_HEAP_TYPE_UPLOAD;
		break;

	case MEMORY_USAGE_GPU_TO_CPU:
		allocationDesc.HeapType = D3D12_HEAP_TYPE_READBACK;
		break;
	}
	allocationDesc.ExtraHeapFlags = D3D12_HEAP_FLAG_NONE;
	allocationDesc.pPrivateData = nullptr;

	D3D12_RESOURCE_STATES initialState = DirectXGetResourecState(pInfo->initialState);
	if (pInfo->usage == MEMORY_USAGE_CPU_ONLY || pInfo->usage == MEMORY_USAGE_CPU_TO_GPU)
	{
		initialState = D3D12_RESOURCE_STATE_GENERIC_READ;
	}

	if (pInfo->usage == MEMORY_USAGE_GPU_TO_CPU)
	{
		initialState = D3D12_RESOURCE_STATE_COPY_DEST;
	}

	DIRECTX_ERROR_CHECK(pRenderer->dx.allocator->CreateResource(&allocationDesc, &resourceDesc,
		initialState, nullptr, &pBuffer->dx.allocation, IID_PPV_ARGS(&pBuffer->dx.resource)));

	if (pInfo->type & BUFFER_TYPE_UNIFORM)
	{
		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{};
		cbvDesc.BufferLocation = pBuffer->dx.resource->GetGPUVirtualAddress();
		cbvDesc.SizeInBytes = allocationSize;

		DirectXDescriptroHeapAllocateInfo heapAllocateInfo{};
		heapAllocateInfo.type = VIEW_TYPE_CBV;
		heapAllocateInfo.pCbvDesc = &cbvDesc;
		heapAllocateInfo.pBuffer = pBuffer;
		heapAllocateInfo.allocateOnGpuHeap = false;
		DirectXDescriptorHeapAllocate(pRenderer, &heapAllocateInfo, &gCbvSrvUavHeap, nullptr);
	}

	if (pInfo->type & BUFFER_TYPE_BUFFER)
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		srvDesc.Buffer.FirstElement = pInfo->firstElement;
		srvDesc.Buffer.NumElements = pInfo->numElements;
		srvDesc.Buffer.StructureByteStride = pInfo->stride;
		srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

		DirectXDescriptroHeapAllocateInfo heapAllocateInfo{};
		heapAllocateInfo.type = VIEW_TYPE_SRV;
		heapAllocateInfo.pSrvDesc = &srvDesc;
		heapAllocateInfo.pBuffer = pBuffer;
		heapAllocateInfo.allocateOnGpuHeap = false;
		DirectXDescriptorHeapAllocate(pRenderer, &heapAllocateInfo, &gCbvSrvUavHeap, nullptr);
	}

	if (pInfo->type & BUFFER_TYPE_RW_BUFFER)
	{
		D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
		uavDesc.Format = DXGI_FORMAT_UNKNOWN;
		uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
		uavDesc.Buffer.FirstElement = pInfo->firstElement;
		uavDesc.Buffer.NumElements = pInfo->numElements;
		uavDesc.Buffer.StructureByteStride = pInfo->stride;
		uavDesc.Buffer.CounterOffsetInBytes = 0;
		uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;

		DirectXDescriptroHeapAllocateInfo heapAllocateInfo{};
		heapAllocateInfo.type = VIEW_TYPE_UAV;
		heapAllocateInfo.pUavDesc = &uavDesc;
		heapAllocateInfo.pBuffer = pBuffer;
		heapAllocateInfo.allocateOnGpuHeap = false;
		DirectXDescriptorHeapAllocate(pRenderer, &heapAllocateInfo, &gCbvSrvUavHeap, nullptr);
	}

	//FOR COPYING, NEED TO USE A NON-COPY QUEUE TO TRANSITION FROM RESOURCE STATE COMMON
	if (copyData)
		DirectXCopyBuffer(pRenderer, pInfo, pBuffer, allocationSize);

	pBuffer->size = pInfo->size;
	pBuffer->type = pInfo->type;
}

void DirectXDestroyBuffer(const Renderer* const pRenderer, Buffer* pBuffer)
{
	if (pBuffer->type & BUFFER_TYPE_UNIFORM)
		DirectXDescriptorHeapFree(&gCbvSrvUavHeap, pBuffer->dx.cpuCbDescriptorId, pBuffer->dx.gpuCbDescriptorId);

	if (pBuffer->type & BUFFER_TYPE_BUFFER)
		DirectXDescriptorHeapFree(&gCbvSrvUavHeap, pBuffer->dx.cpuSrvDescriptorId, pBuffer->dx.gpuSrvDescriptorId);

	if (pBuffer->type & BUFFER_TYPE_RW_BUFFER)
		DirectXDescriptorHeapFree(&gCbvSrvUavHeap, pBuffer->dx.cpuUavDescriptorId, pBuffer->dx.gpuUavDescriptorId);

	SAFE_RELEASE(pBuffer->dx.resource);
	SAFE_RELEASE(pBuffer->dx.allocation);
}

void DirectXBindVertexBuffer(const CommandBuffer* const pCommandBuffer, const uint32_t stride, const uint32_t bindingLocation,
	const uint32_t offset, const Buffer* const pVertexBuffer)
{
	D3D12_VERTEX_BUFFER_VIEW view{};
	view.BufferLocation = pVertexBuffer->dx.resource->GetGPUVirtualAddress();
	view.SizeInBytes = pVertexBuffer->size;
	view.StrideInBytes = stride;

	pCommandBuffer->dx.commandList->IASetVertexBuffers(0, 1, &view);
}

void DirectXBindIndexBuffer(const CommandBuffer* const pCommandBuffer, const uint32_t offset,
	const IndexType indexType, const Buffer* const pIndexBuffer)
{
	D3D12_INDEX_BUFFER_VIEW view{};
	view.BufferLocation = pIndexBuffer->dx.resource->GetGPUVirtualAddress();
	view.SizeInBytes = pIndexBuffer->size;
	view.Format = (indexType == INDEX_TYPE_UINT16) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;

	pCommandBuffer->dx.commandList->IASetIndexBuffer(&view);
}

void DirectXCopyTexture(const Renderer* const pRenderer, const TextureDesc* const texDesc,
	const Texture* const pDstTexture, uint32_t numBytes)
{
	DirectXCreateStagingBuffer(pRenderer, numBytes);

	void* data = nullptr;
	DirectXMapMemory(pRenderer, &gDirectXCopyEngine.stagingBuffer, &data);

	DirectXBeginCopyEngineCommandList(pRenderer);

	BarrierInfo barrierInfo{};
	barrierInfo.type = BARRIER_TYPE_BUFFER;
	barrierInfo.pBuffer = &gDirectXCopyEngine.stagingBuffer;
	barrierInfo.currentState = RESOURCE_STATE_COMMON;
	barrierInfo.newState = RESOURCE_STATE_COPY_SOURCE;
	DirectXResourceBarrier(&gDirectXCopyEngine.copyCommandBuffer, 1, &barrierInfo);

	barrierInfo.type = BARRIER_TYPE_BUFFER;
	barrierInfo.pTexture = pDstTexture;
	barrierInfo.currentState = RESOURCE_STATE_COMMON;
	barrierInfo.newState = RESOURCE_STATE_COPY_DEST;
	DirectXResourceBarrier(&gDirectXCopyEngine.copyCommandBuffer, 1, &barrierInfo);

	uint32_t numImages = texDesc->arraySize * texDesc->mipCount;
	uint32_t offset = 0;
	for (uint32_t i = 0; i < numImages; ++i)
	{
		memcpy((uint8_t*)data + offset, texDesc->images[i].data, texDesc->images[i].numBytes);
		offset += texDesc->images[i].numBytes;
	}

	D3D12_RESOURCE_DESC resourceDesc = pDstTexture->dx.resource->GetDesc();
	D3D12_PLACED_SUBRESOURCE_FOOTPRINT* footprints = (D3D12_PLACED_SUBRESOURCE_FOOTPRINT*)calloc(numImages, sizeof(D3D12_PLACED_SUBRESOURCE_FOOTPRINT));
	pRenderer->dx.device->GetCopyableFootprints(&resourceDesc, 0, numImages, 0, footprints, nullptr, nullptr, nullptr);
	for (uint32_t i = 0; i < numImages; ++i)
	{
		D3D12_TEXTURE_COPY_LOCATION src{};
		src.pResource = gDirectXCopyEngine.stagingBuffer.dx.resource;
		src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
		src.PlacedFootprint = footprints[i];

		D3D12_TEXTURE_COPY_LOCATION dst{};
		dst.pResource = pDstTexture->dx.resource;
		dst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
		dst.SubresourceIndex = i;
		
		gDirectXCopyEngine.copyCommandBuffer.dx.commandList->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);
	}

	DirectXEndCopyEngineCommandList();

	DirectXCopyEngineSubmit(pRenderer);

	BarrierInfo barrier{};
	barrier.type = BARRIER_TYPE_TEXTURE;
	barrier.pTexture = pDstTexture;
	barrier.currentState = RESOURCE_STATE_COPY_DEST;
	barrier.newState = RESOURCE_STATE_ALL_SHADER_RESOURCE;
	DirectXInitialTransition(pRenderer, &barrier);

	DirectXUnmapMemory(pRenderer, &gDirectXCopyEngine.stagingBuffer);
	DirectXDestroyStagingBuffer(pRenderer);

	free(footprints);
}

DXGI_FORMAT GetUavFormat(DXGI_FORMAT format)
{
	switch (format)
	{
	case DXGI_FORMAT_R8G8B8A8_TYPELESS:
	case DXGI_FORMAT_R8G8B8A8_UNORM:
	case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
		return DXGI_FORMAT_R8G8B8A8_UNORM;

	case DXGI_FORMAT_B8G8R8A8_TYPELESS:
	case DXGI_FORMAT_B8G8R8A8_UNORM:
	case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
		return DXGI_FORMAT_B8G8R8A8_UNORM;

	case DXGI_FORMAT_B8G8R8X8_TYPELESS:
	case DXGI_FORMAT_B8G8R8X8_UNORM:
	case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
		return DXGI_FORMAT_B8G8R8X8_UNORM;

	case DXGI_FORMAT_R32_TYPELESS:
	case DXGI_FORMAT_R32_FLOAT:
		return DXGI_FORMAT_R32_FLOAT;

	default:
		return format;
	}
}

DXGI_FORMAT GetSrvFormat(DXGI_FORMAT format)
{
	switch (format)
	{
	// 32-bit with Stencil
	case DXGI_FORMAT_R32G8X24_TYPELESS:
	case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
	case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
	case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
		return DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;

	//No Stencil
	case DXGI_FORMAT_R32_TYPELESS:
	case DXGI_FORMAT_D32_FLOAT:
	case DXGI_FORMAT_R32_FLOAT:
		return DXGI_FORMAT_R32_FLOAT;

	//24-bit with stencil
	case DXGI_FORMAT_R24G8_TYPELESS:
	case DXGI_FORMAT_D24_UNORM_S8_UINT:
	case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
	case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
		return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;

	//16-bit without Stencil
	case DXGI_FORMAT_R16_TYPELESS:
	case DXGI_FORMAT_D16_UNORM:
	case DXGI_FORMAT_R16_UNORM:
		return DXGI_FORMAT_R16_UNORM;

	case DXGI_FORMAT_R8G8B8A8_TYPELESS:
		return DXGI_FORMAT_R8G8B8A8_UNORM;

	default:
		return format;
	}
}

void DirectXCreateTexture(const Renderer* const pRenderer, const TextureInfo* const pInfo, Texture* pTexture)
{
	D3D12_RESOURCE_DESC resourceDesc{};
	TextureDesc texDesc{};
	uint32_t texType{};
	if (pInfo->filename != nullptr)
	{
		uint8_t* bitData = nullptr;
		uint32_t numBytes = 0;
		DDS_HEADER ddsHeader{};
		DDS_HEADER_DXT10 ddsHeader10{};
		ReadDDSFile(pInfo->filename, &bitData, &numBytes, &ddsHeader, &ddsHeader10);

		int result = RetrieveTextureInfo(&ddsHeader, &ddsHeader10, bitData, numBytes, &texDesc);
		if (result != SE_SUCCESS)
		{
			MessageBox(nullptr, L"Error with function RetrieveTextureInfo in function DirectXCreateTexture. Exiting Program.",
				L"RetrieveTextureInfo Error", MB_OK);
			SAFE_FREE(bitData);
			exit(5);
		}

		DXGI_FORMAT format = (DXGI_FORMAT)TinyImageFormat_ToDXGI_FORMAT(texDesc.format);

		switch (texDesc.resDim)
		{
		case TEXTURE_DIMENSION_1D:
			resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE1D;
			break;
		case TEXTURE_DIMENSION_2D:
			resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
			break;
		case TEXTURE_DIMENSION_3D:
			resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE3D;
			break;
		}
		resourceDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
		resourceDesc.Width = texDesc.width;
		resourceDesc.Height = texDesc.height;
		resourceDesc.DepthOrArraySize = (texDesc.arraySize != 1) ? texDesc.arraySize : texDesc.depth;
		resourceDesc.MipLevels = texDesc.mipCount;
		resourceDesc.Format = format;
		resourceDesc.SampleDesc.Count = 1;
		resourceDesc.SampleDesc.Quality = 0;
		resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		D3D12MA::ALLOCATION_DESC allocationDesc{};
		allocationDesc.Flags = D3D12MA::ALLOCATION_FLAG_NONE;
		allocationDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;
		allocationDesc.ExtraHeapFlags = D3D12_HEAP_FLAG_NONE;
		allocationDesc.pPrivateData = nullptr;

		D3D12_RESOURCE_STATES initialState = D3D12_RESOURCE_STATE_COMMON;

		DIRECTX_ERROR_CHECK(pRenderer->dx.allocator->CreateResource(&allocationDesc, &resourceDesc,
			initialState, nullptr, &pTexture->dx.allocation, IID_PPV_ARGS(&pTexture->dx.resource)));

		numBytes = pTexture->dx.allocation->GetSize();
		DirectXCopyTexture(pRenderer, &texDesc, pTexture, numBytes);

		SAFE_FREE(bitData);
		SAFE_FREE(texDesc.images);

		texType = TEXTURE_TYPE_TEXTURE;
	}
	else
	{
		bool const isDepth = TinyImageFormat_IsDepthOnly(pInfo->format) || TinyImageFormat_IsDepthAndStencil(pInfo->format);

		switch (pInfo->dimension)
		{
		case TEXTURE_DIMENSION_1D:
			resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE1D;
			break;
		case TEXTURE_DIMENSION_2D:
			resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
			break;
		case TEXTURE_DIMENSION_3D:
			resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE3D;
			break;
		}
		resourceDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
		resourceDesc.Width = pInfo->width;
		resourceDesc.Height = pInfo->height;
		resourceDesc.DepthOrArraySize = (pInfo->arraySize != 1) ? pInfo->arraySize : pInfo->depth;
		resourceDesc.MipLevels = pInfo->mipCount;
		resourceDesc.Format = (DXGI_FORMAT)TinyImageFormat_ToDXGI_FORMAT(pInfo->format);
		resourceDesc.SampleDesc.Count = 1;
		resourceDesc.SampleDesc.Quality = 0;
		resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
		
		if (pInfo->type & TEXTURE_TYPE_RW_TEXTURE)
			resourceDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

		if (pInfo->isRenderTarget == true)
		{
			if (isDepth)
				resourceDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
			else
				resourceDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
		}

		D3D12_CLEAR_VALUE clearValue{};
		clearValue.Format = resourceDesc.Format;
		
		if (isDepth)
		{
			clearValue.DepthStencil.Depth = pInfo->clearValue.depth;
			clearValue.DepthStencil.Stencil = pInfo->clearValue.stencil;
		}
		else
		{
			clearValue.Color[0] = pInfo->clearValue.r;
			clearValue.Color[1] = pInfo->clearValue.g;
			clearValue.Color[2] = pInfo->clearValue.b;
			clearValue.Color[3] = pInfo->clearValue.a;
		}

		D3D12_CLEAR_VALUE* pClearValue = nullptr;
		if (pInfo->isRenderTarget)
			pClearValue = &clearValue;
		

		D3D12MA::ALLOCATION_DESC allocationDesc{};
		allocationDesc.Flags = D3D12MA::ALLOCATION_FLAG_NONE;
		allocationDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;
		allocationDesc.ExtraHeapFlags = D3D12_HEAP_FLAG_NONE;
		allocationDesc.pPrivateData = nullptr;

		if (pInfo->isRenderTarget)
		{
			pClearValue = &clearValue;
			allocationDesc.Flags = D3D12MA::ALLOCATION_FLAG_COMMITTED;
		}

		D3D12_RESOURCE_STATES initialState = DirectXGetResourecState(pInfo->initialState);

		DIRECTX_ERROR_CHECK(pRenderer->dx.allocator->CreateResource(&allocationDesc, &resourceDesc,
			initialState, pClearValue, &pTexture->dx.allocation, IID_PPV_ARGS(&pTexture->dx.resource)));

		texType = pInfo->type;
	}

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = GetSrvFormat(resourceDesc.Format);
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
	uavDesc.Format = GetUavFormat(resourceDesc.Format);

	switch (resourceDesc.Dimension)
	{
	case D3D12_RESOURCE_DIMENSION_TEXTURE1D:
		if (resourceDesc.DepthOrArraySize > 1)
		{
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1DARRAY;
			srvDesc.Texture1DArray.ArraySize = resourceDesc.DepthOrArraySize;
			srvDesc.Texture1DArray.FirstArraySlice = 0;
			srvDesc.Texture1DArray.MipLevels = resourceDesc.MipLevels;
			srvDesc.Texture1DArray.MostDetailedMip = 0;

			uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE1DARRAY;
			uavDesc.Texture1DArray.ArraySize = resourceDesc.DepthOrArraySize;
			uavDesc.Texture1DArray.FirstArraySlice = 0;
			uavDesc.Texture1DArray.MipSlice = 0;
		}
		else
		{
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1D;
			srvDesc.Texture1D.MipLevels = resourceDesc.MipLevels;
			srvDesc.Texture1D.MostDetailedMip = 0;
			srvDesc.Texture1D.ResourceMinLODClamp = 0.0f;

			uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE1D;
			uavDesc.Texture1D.MipSlice = 0;
		}
		break;

	case D3D12_RESOURCE_DIMENSION_TEXTURE2D:
	{
		bool isCubeMap = (pInfo->filename == nullptr) ? pInfo->isCubeMap : texDesc.isCubeMap;
		if (isCubeMap)
		{
			if (resourceDesc.DepthOrArraySize > 6)
			{
				srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBEARRAY;
				srvDesc.TextureCubeArray.MostDetailedMip = 0;
				srvDesc.TextureCubeArray.MipLevels = resourceDesc.MipLevels;
				srvDesc.TextureCubeArray.First2DArrayFace = 0;
				srvDesc.TextureCubeArray.NumCubes = resourceDesc.DepthOrArraySize / 6;
			}
			else
			{
				srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
				srvDesc.TextureCube.MostDetailedMip = 0;
				srvDesc.TextureCube.MipLevels = resourceDesc.MipLevels;
				srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;
			}

			uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
			uavDesc.Texture2DArray.ArraySize = resourceDesc.DepthOrArraySize;
			uavDesc.Texture2DArray.FirstArraySlice = 0;
			uavDesc.Texture2DArray.MipSlice = 0;
			uavDesc.Texture2DArray.PlaneSlice = 0;

		}
		else
		{
			if (resourceDesc.DepthOrArraySize > 1)
			{
				srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
				srvDesc.Texture2DArray.MostDetailedMip = 0;
				srvDesc.Texture2DArray.MipLevels = resourceDesc.MipLevels;
				srvDesc.Texture2DArray.FirstArraySlice = 0;
				srvDesc.Texture2DArray.ArraySize = resourceDesc.DepthOrArraySize;
				srvDesc.Texture2DArray.PlaneSlice = 0;
				srvDesc.Texture2DArray.ResourceMinLODClamp = 0.0f;

				uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
				uavDesc.Texture2DArray.ArraySize = resourceDesc.DepthOrArraySize;
				uavDesc.Texture2DArray.FirstArraySlice = 0;
				uavDesc.Texture2DArray.MipSlice = 0;
				uavDesc.Texture2DArray.PlaneSlice = 0;
			}
			else
			{
				srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
				srvDesc.Texture2D.MostDetailedMip = 0;
				srvDesc.Texture2D.MipLevels = resourceDesc.MipLevels;
				srvDesc.Texture2D.PlaneSlice = 0;
				srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

				uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
				uavDesc.Texture2D.MipSlice = 0;
				uavDesc.Texture2D.PlaneSlice = 0;
			}
		}
		break;
	}

	case D3D12_RESOURCE_DIMENSION_TEXTURE3D:
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE3D;
		srvDesc.Texture3D.MostDetailedMip = 0;
		srvDesc.Texture3D.MipLevels = resourceDesc.MipLevels;
		srvDesc.Texture3D.ResourceMinLODClamp = 0.0f;

		uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE3D;
		uavDesc.Texture3D.FirstWSlice = 0;
		uavDesc.Texture3D.MipSlice = 0;
		uavDesc.Texture3D.WSize = resourceDesc.DepthOrArraySize;

		break;
	}

	if (texType & TEXTURE_TYPE_TEXTURE)
	{
		DirectXDescriptroHeapAllocateInfo heapAllocateInfo{};
		heapAllocateInfo.type = VIEW_TYPE_SRV;
		heapAllocateInfo.pSrvDesc = &srvDesc;
		heapAllocateInfo.pTexture = pTexture;
		heapAllocateInfo.allocateOnGpuHeap = false;
		DirectXDescriptorHeapAllocate(pRenderer, &heapAllocateInfo, &gCbvSrvUavHeap, nullptr);
	}

	if(texType & TEXTURE_TYPE_RW_TEXTURE)
	{
		DirectXDescriptroHeapAllocateInfo heapAllocateInfo{};
		heapAllocateInfo.type = VIEW_TYPE_UAV;
		heapAllocateInfo.pUavDesc = &uavDesc;
		heapAllocateInfo.pTexture = pTexture;
		heapAllocateInfo.allocateOnGpuHeap = false;
		DirectXDescriptorHeapAllocate(pRenderer, &heapAllocateInfo, &gCbvSrvUavHeap, nullptr);
	}

	pTexture->type = texType;
}

void DirectXDestroyTexture(const Renderer* const pRenderer, Texture* pTexture)
{
	if (pTexture->type & TEXTURE_TYPE_TEXTURE)
		DirectXDescriptorHeapFree(&gCbvSrvUavHeap, pTexture->dx.cpuSrvDescriptorId, pTexture->dx.gpuSrvDescriptorId);
	
	if (pTexture->type & TEXTURE_TYPE_RW_TEXTURE)
		DirectXDescriptorHeapFree(&gCbvSrvUavHeap, pTexture->dx.cpuUavDescriptorId, pTexture->dx.gpuUavDescriptorId);

	SAFE_RELEASE(pTexture->dx.resource);
	SAFE_RELEASE(pTexture->dx.allocation);
}

D3D12_FILTER DirectXGetFilter(Filter min, Filter mag, MipMapMode mode, DepthFunction comparisonFunc, float anisotropy)
{
	if (comparisonFunc == DEPTH_FUNCTION_NONE)
	{
		if (anisotropy > 0.0f)
			return D3D12_FILTER_ANISOTROPIC;

		if (min == FILTER_NEAREST && mag == FILTER_NEAREST && mode == MIPMAP_MODE_NEAREST)
			return D3D12_FILTER_MIN_MAG_MIP_POINT;

		if (min == FILTER_NEAREST && mag == FILTER_NEAREST && mode == MIPMAP_MODE_LINEAR)
			return D3D12_FILTER_MIN_MAG_POINT_MIP_LINEAR;

		if (min == FILTER_NEAREST && mag == FILTER_LINEAR && mode == MIPMAP_MODE_NEAREST)
			return D3D12_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;

		if (min == FILTER_NEAREST && mag == FILTER_LINEAR && mode == MIPMAP_MODE_LINEAR)
			return D3D12_FILTER_MIN_POINT_MAG_MIP_LINEAR;

		if (min == FILTER_LINEAR && mag == FILTER_NEAREST && mode == MIPMAP_MODE_NEAREST)
			return D3D12_FILTER_MIN_LINEAR_MAG_MIP_POINT;

		if (min == FILTER_LINEAR && mag == FILTER_NEAREST && mode == MIPMAP_MODE_LINEAR)
			return D3D12_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;

		if (min == FILTER_LINEAR && mag == FILTER_LINEAR && mode == MIPMAP_MODE_NEAREST)
			return D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT;

		if (min == FILTER_LINEAR && mag == FILTER_LINEAR && mode == MIPMAP_MODE_LINEAR)
			return D3D12_FILTER_MIN_MAG_MIP_LINEAR;

		return D3D12_FILTER_MIN_MAG_MIP_POINT;
	}
	else
	{
		if (anisotropy > 0.0f)
			return D3D12_FILTER_COMPARISON_ANISOTROPIC;

		if (min == FILTER_NEAREST && mag == FILTER_NEAREST && mode == MIPMAP_MODE_NEAREST)
			return D3D12_FILTER_COMPARISON_MIN_MAG_MIP_POINT;

		if (min == FILTER_NEAREST && mag == FILTER_NEAREST && mode == MIPMAP_MODE_LINEAR)
			return D3D12_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR;

		if (min == FILTER_NEAREST && mag == FILTER_LINEAR && mode == MIPMAP_MODE_NEAREST)
			return D3D12_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT;

		if (min == FILTER_NEAREST && mag == FILTER_LINEAR && mode == MIPMAP_MODE_LINEAR)
			return D3D12_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR;

		if (min == FILTER_LINEAR && mag == FILTER_NEAREST && mode == MIPMAP_MODE_NEAREST)
			return D3D12_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT;

		if (min == FILTER_LINEAR && mag == FILTER_NEAREST && mode == MIPMAP_MODE_LINEAR)
			return D3D12_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR;

		if (min == FILTER_LINEAR && mag == FILTER_LINEAR && mode == MIPMAP_MODE_NEAREST)
			return D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;

		if (min == FILTER_LINEAR && mag == FILTER_LINEAR && mode == MIPMAP_MODE_LINEAR)
			return D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;

		return D3D12_FILTER_COMPARISON_MIN_MAG_MIP_POINT;
	}
}

D3D12_TEXTURE_ADDRESS_MODE DirectXGetAddressMode(AddressMode addressMode)
{
	switch (addressMode)
	{
	case ADDRESS_MODE_REPEAT:
		return D3D12_TEXTURE_ADDRESS_MODE_WRAP;

	case ADDRESS_MODE_MIRRORED:
		return D3D12_TEXTURE_ADDRESS_MODE_MIRROR;

	case ADDRESS_MODE_CLAMP_TO_EDGE:
		return D3D12_TEXTURE_ADDRESS_MODE_CLAMP;

	case ADDRESS_MODE_CLAMP_TO_BORDER:
		return D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	}

	return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
}

void DirectXCreateSampler(const Renderer* const pRenderer, const SamplerInfo* const pInfo, Sampler* pSampler)
{
	D3D12_SAMPLER_DESC samplerDesc{};
	samplerDesc.Filter = DirectXGetFilter(pInfo->minFilter, pInfo->magFilter, pInfo->mipMapMode, 
		pInfo->comparisonFunction, pInfo->maxAnisotropy);

	samplerDesc.AddressU = DirectXGetAddressMode(pInfo->u);
	samplerDesc.AddressV = DirectXGetAddressMode(pInfo->v);
	samplerDesc.AddressW = DirectXGetAddressMode(pInfo->w);
	samplerDesc.MipLODBias = pInfo->mipLoadBias;
	samplerDesc.MaxAnisotropy = pInfo->maxAnisotropy;
	samplerDesc.ComparisonFunc = GetDepthFunction(pInfo->comparisonFunction);
	samplerDesc.BorderColor[0] = pInfo->borderColor[0];
	samplerDesc.BorderColor[1] = pInfo->borderColor[1];
	samplerDesc.BorderColor[2] = pInfo->borderColor[2];
	samplerDesc.BorderColor[3] = pInfo->borderColor[3];
	samplerDesc.MinLOD = pInfo->minLod;
	samplerDesc.MaxLOD = pInfo->maxLod;

	DirectXDescriptroHeapAllocateInfo heapAllocateInfo{};
	heapAllocateInfo.type = VIEW_TYPE_SAMPLER;
	heapAllocateInfo.pSamplerDesc = &samplerDesc;
	heapAllocateInfo.pSampler = pSampler;
	heapAllocateInfo.allocateOnGpuHeap = false;
	DirectXDescriptorHeapAllocate(pRenderer, &heapAllocateInfo, &gSamplerHeap, nullptr);
}

void DirectXDestroySampler(const Renderer* const pRenderer, Sampler* pSampler)
{
	DirectXDescriptorHeapFree(&gSamplerHeap, pSampler->dx.cpuDescriptorId, pSampler->dx.gpuDescriptorId);
}

void DirectXCreateDescriptorSet(const Renderer* const pRenderer, const DescriptorSetInfo* const pInfo, DescriptorSet* pDescriptorSet)
{
	pDescriptorSet->updateFrequency = pInfo->updateFrequency;
	pDescriptorSet->pRootSignature = pInfo->pRootSignature;

	pDescriptorSet->dx.heapIndices = (int32_t*)calloc(pInfo->numSets, sizeof(int32_t));
	pDescriptorSet->dx.samplerHeapIndices = (int32_t*)calloc(pInfo->numSets, sizeof(int32_t));

	for (uint32_t i = 0; i < pInfo->numSets; ++i)
	{
		pDescriptorSet->dx.heapIndices[i] = -1;
		pDescriptorSet->dx.samplerHeapIndices[i] = -1;
	}
}

void DirectXDestroyDescriptorSet(DescriptorSet* pDescriptorSet)
{
	SAFE_FREE(pDescriptorSet->dx.samplerHeapIndices);
	SAFE_FREE(pDescriptorSet->dx.heapIndices);
}

void DirectXUpdateDescriptorSet(const Renderer* const pRenderer, const DescriptorSet* const pDescriptorSet, uint32_t index,
	const uint32_t numInfos, const UpdateDescriptorSetInfo* const pInfos)
{
	uint32_t firstIndex = 0;
	for (uint32_t i = 0; i < numInfos; ++i)
	{
		DirectXDescriptroHeapAllocateInfo heapAllocateInfo{};
		if (pInfos[i].type != UPDATE_TYPE_SAMPLER)
		{
			if (pInfos[i].type == UPDATE_TYPE_UNIFORM_BUFFER)
			{
				heapAllocateInfo.type = VIEW_TYPE_CBV;
				heapAllocateInfo.pCbvDesc = nullptr;
				heapAllocateInfo.pBuffer = pInfos[i].pBuffer;
				heapAllocateInfo.allocateOnGpuHeap = true;
				heapAllocateInfo.isBuffer = true;
			}
			else if (pInfos[i].type == UPDATE_TYPE_BUFFER)
			{
				heapAllocateInfo.type = VIEW_TYPE_SRV;
				heapAllocateInfo.pSrvDesc = nullptr;
				heapAllocateInfo.pBuffer = pInfos[i].pBuffer;
				heapAllocateInfo.allocateOnGpuHeap = true;
				heapAllocateInfo.isBuffer = true;
			}
			else if (pInfos[i].type == UPDATE_TYPE_RW_BUFFER)
			{
				heapAllocateInfo.type = VIEW_TYPE_UAV;
				heapAllocateInfo.pUavDesc = nullptr;
				heapAllocateInfo.pBuffer = pInfos[i].pBuffer;
				heapAllocateInfo.allocateOnGpuHeap = true;
				heapAllocateInfo.isBuffer = true;
			}
			else if (pInfos[i].type == UPDATE_TYPE_TEXTURE)
			{
				heapAllocateInfo.type = VIEW_TYPE_SRV;
				heapAllocateInfo.pSrvDesc = nullptr;
				heapAllocateInfo.pTexture = pInfos[i].pTexture;
				heapAllocateInfo.allocateOnGpuHeap = true;
				heapAllocateInfo.isBuffer = false;
			}
			else //UPDATE_TYPE_RW_TEXTURE
			{
				heapAllocateInfo.type = VIEW_TYPE_UAV;
				heapAllocateInfo.pUavDesc = nullptr;
				heapAllocateInfo.pTexture = pInfos[i].pTexture;
				heapAllocateInfo.allocateOnGpuHeap = true;
				heapAllocateInfo.isBuffer = false;
			}

			DirectXDescriptorHeapAllocate(pRenderer, &heapAllocateInfo, &gCbvSrvUavHeap, &firstIndex);

			if (pDescriptorSet->dx.heapIndices[index] < 0)
				pDescriptorSet->dx.heapIndices[index] = firstIndex;
		}
		else //UPDATE_TYPE_SAMPLER
		{
			heapAllocateInfo.type = VIEW_TYPE_SAMPLER;
			heapAllocateInfo.pSamplerDesc = nullptr;
			heapAllocateInfo.pSampler = pInfos[i].pSampler;
			heapAllocateInfo.allocateOnGpuHeap = true;
			DirectXDescriptorHeapAllocate(pRenderer, &heapAllocateInfo, &gSamplerHeap, &firstIndex);

			if (pDescriptorSet->dx.samplerHeapIndices[index] < 0)
				pDescriptorSet->dx.samplerHeapIndices[index] = firstIndex;
		}
	}
}

void DirectXBindDescriptorSet(const CommandBuffer* const pCommandBuffer, const uint32_t index, const uint32_t firstSet,
	const DescriptorSet* const pDescriptorSet)
{
	if (pDescriptorSet->pRootSignature->pipelineType == PIPELINE_TYPE_GRAPHICS)
	{
		if (pDescriptorSet->dx.heapIndices[index] >= 0)
		{
			D3D12_GPU_DESCRIPTOR_HANDLE cbvSrvUavHandle = gCbvSrvUavHeap.gpuHeap->GetGPUDescriptorHandleForHeapStart();
			cbvSrvUavHandle.ptr += (pDescriptorSet->dx.heapIndices[index] * gCbvSrvUavHeap.descriptorSize);
			pCommandBuffer->dx.commandList->SetGraphicsRootDescriptorTable(
				pDescriptorSet->pRootSignature->dx.rootParamterIndices[pDescriptorSet->updateFrequency],
				cbvSrvUavHandle);
		}

		if (pDescriptorSet->dx.samplerHeapIndices[index] >= 0)
		{
			D3D12_GPU_DESCRIPTOR_HANDLE samplerHandle = gSamplerHeap.gpuHeap->GetGPUDescriptorHandleForHeapStart();
			samplerHandle.ptr += (pDescriptorSet->dx.samplerHeapIndices[index] * gSamplerHeap.descriptorSize);

			pCommandBuffer->dx.commandList->SetGraphicsRootDescriptorTable(
				pDescriptorSet->pRootSignature->dx.rootParameterSamplerIndex, samplerHandle);
		}
	}
	else // PIPELINE_TYPE_COMPUTE
	{
		if (pDescriptorSet->dx.heapIndices[index] >= 0)
		{
			D3D12_GPU_DESCRIPTOR_HANDLE cbvSrvUavHandle = gCbvSrvUavHeap.gpuHeap->GetGPUDescriptorHandleForHeapStart();
			cbvSrvUavHandle.ptr += (pDescriptorSet->dx.heapIndices[index] * gCbvSrvUavHeap.descriptorSize);
			pCommandBuffer->dx.commandList->SetComputeRootDescriptorTable(
				pDescriptorSet->pRootSignature->dx.rootParamterIndices[pDescriptorSet->updateFrequency],
				cbvSrvUavHandle);
		}
	}
}

void DirectXBindRootConstants(const CommandBuffer* const pCommandBuffer, uint32_t numValues, uint32_t stride, const void* pData, uint32_t offset)
{
	if (pCommandBuffer->pCurrentPipeline->type == PIPELINE_TYPE_GRAPHICS)
	{
		pCommandBuffer->dx.commandList->SetGraphicsRoot32BitConstants(pCommandBuffer->pCurrentPipeline->pRootSignature->dx.rootConstantsIndex,
			numValues, pData, offset);
	}
	else //PIPELINE_TYPE_COMPUTE
	{
		pCommandBuffer->dx.commandList->SetComputeRoot32BitConstants(pCommandBuffer->pCurrentPipeline->pRootSignature->dx.rootConstantsIndex,
			numValues, pData, offset);
	}
}