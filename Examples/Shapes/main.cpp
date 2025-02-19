#include "../../../SecondEngine/SEApp.h"
#include "../../../SecondEngine/Renderer/SERenderer.h"
#include "../../../SecondEngine/Math/SEMath_Header.h"
#include "../../../SecondEngine/Renderer/SECamera.h"
#include "../../../SecondEngine/Time/SETimer.h"
#include "../../../SecondEngine/Shapes/SEShapes.h"

Renderer gRenderer;

Queue gGraphicsQueue;

SwapChain gSwapChain;

RenderTarget gDepthBuffer;

Shader gVertexShader;
Shader gPixelShader;
RootSignature gGraphicsRootSignature;
Pipeline gGraphicsPipeline;

const uint32_t gNumFrames = 2;
Semaphore gImageAvailableSemaphores[gNumFrames];
CommandBuffer gGraphicsCommandBuffers[gNumFrames];

Buffer gVertexBuffer;
Buffer gIndexBuffer;

Buffer gPerFrameBuffer[gNumFrames];

Buffer gEqTriangleUniformBuffer[gNumFrames];
Buffer gRightTriangleUniformBuffer[gNumFrames];
Buffer gQuadUniformBuffer[gNumFrames];
Buffer gCircleUniformBuffer[gNumFrames];

Texture gStatueTexture;
Sampler gStatueSampler;

uint32_t gCurrentFrame = 0;

DescriptorSet gDescriptorSetUniforms;
DescriptorSet gDescriptorSetTextures;
DescriptorSet gDescriptorSetSamplers;

Camera gCamera;

Vertex* gVertices = nullptr;
uint32_t* gIndices = nullptr;

struct PerObjectUniformData
{
	mat4 model;
};

struct PerFrameUniformData
{
	mat4 view;
	mat4 projection;
};

PerFrameUniformData gPerFrameUniformData;

enum
{
	EQUILATERAL_TRIANGLE,
	RIGHT_TRIANGLE,
	QUAD,
	CIRCLE,
	MAX_SHAPES
};

PerObjectUniformData gShapesUniformData[MAX_SHAPES];

uint32_t gVertexOffsets[MAX_SHAPES];
uint32_t gIndexOffsets[MAX_SHAPES];
uint32_t gIndexCounts[MAX_SHAPES];

mat4 gCubeRotation;
float gAngle = 0.0f;

class Test : public App
{
public:
	void Init() override
	{
		InitSE();

		InitRenderer(&gRenderer, "Test");

		CreateQueue(&gRenderer, QUEUE_TYPE_GRAPHICS, &gGraphicsQueue);

		SwapChainInfo swapChainInfo{};
		swapChainInfo.window = pWindow;
		swapChainInfo.queue = &gGraphicsQueue;
		swapChainInfo.width = GetWidth(pWindow);
		swapChainInfo.height = GetHeight(pWindow);
		swapChainInfo.format = TinyImageFormat_B8G8R8A8_UNORM;
		swapChainInfo.clearValue.r = 0.0f;
		swapChainInfo.clearValue.g = 0.5f;
		swapChainInfo.clearValue.b = 0.5f;
		swapChainInfo.clearValue.a = 1.0f;
		CreateSwapChain(&gRenderer, &swapChainInfo, &gSwapChain);

		RenderTargetInfo dbInfo{};
		dbInfo.format = TinyImageFormat_D32_SFLOAT;
		dbInfo.width = GetWidth(pWindow);
		dbInfo.height = GetHeight(pWindow);
		dbInfo.clearValue.depth = 1.0f;
		dbInfo.clearValue.stencil = 0;
		dbInfo.initialState = RESOURCE_STATE_DEPTH_WRITE;
		CreateRenderTarget(&gRenderer, &dbInfo, &gDepthBuffer);

		ShaderInfo vertexShaderInfo{};
		vertexShaderInfo.glslFilename = "CompiledShaders/shapesVS.spv";
		vertexShaderInfo.hlslFilename = "CompiledShaders/shapesVS.cso";
		vertexShaderInfo.type = SHADER_TYPE_VERTEX;
		CreateShader(&gRenderer, &vertexShaderInfo, &gVertexShader);

		ShaderInfo pixelShaderInfo{};
		pixelShaderInfo.glslFilename = "CompiledShaders/shapesPS.spv";
		pixelShaderInfo.hlslFilename = "CompiledShaders/shapesPS.cso";
		pixelShaderInfo.type = SHADER_TYPE_PIXEL;
		CreateShader(&gRenderer, &pixelShaderInfo, &gPixelShader);

		VertexInputInfo vertexInputInfo{};
		vertexInputInfo.vertexBinding.binding = 0;
		vertexInputInfo.vertexBinding.stride = sizeof(Vertex);
		vertexInputInfo.vertexBinding.inputRate = INPUT_RATE_PER_VERTEX;

		vertexInputInfo.vertexAttributes[0].binding = 0;
		vertexInputInfo.vertexAttributes[0].location = 0;
		vertexInputInfo.vertexAttributes[0].semanticName = "POSITION";
		vertexInputInfo.vertexAttributes[0].semanticIndex = 0;
		vertexInputInfo.vertexAttributes[0].format = TinyImageFormat_R32G32B32A32_SFLOAT;
		vertexInputInfo.vertexAttributes[0].offset = 0;

		vertexInputInfo.vertexAttributes[1].binding = 0;
		vertexInputInfo.vertexAttributes[1].location = 1;
		vertexInputInfo.vertexAttributes[1].semanticName = "NORMAL";
		vertexInputInfo.vertexAttributes[1].semanticIndex = 0;
		vertexInputInfo.vertexAttributes[1].format = TinyImageFormat_R32G32B32A32_SFLOAT;
		vertexInputInfo.vertexAttributes[1].offset = 16;

		vertexInputInfo.vertexAttributes[2].binding = 0;
		vertexInputInfo.vertexAttributes[2].location = 2;
		vertexInputInfo.vertexAttributes[2].semanticName = "TEXCOORD";
		vertexInputInfo.vertexAttributes[2].semanticIndex = 0;
		vertexInputInfo.vertexAttributes[2].format = TinyImageFormat_R32G32_SFLOAT;
		vertexInputInfo.vertexAttributes[2].offset = 32;

		vertexInputInfo.numVertexAttributes = 3;

		RootSignatureInfo graphicsRootSignatureInfo{};
		graphicsRootSignatureInfo.rootParameterInfos[0].binding = 0;
		graphicsRootSignatureInfo.rootParameterInfos[0].baseRegister = 0;
		graphicsRootSignatureInfo.rootParameterInfos[0].registerSpace = 0;
		graphicsRootSignatureInfo.rootParameterInfos[0].numDescriptors = 1;
		graphicsRootSignatureInfo.rootParameterInfos[0].stages = STAGE_VERTEX;
		graphicsRootSignatureInfo.rootParameterInfos[0].type = DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		graphicsRootSignatureInfo.rootParameterInfos[0].updateFrequency = UPDATE_FREQUENCY_PER_FRAME;

		graphicsRootSignatureInfo.rootParameterInfos[1].binding = 1;
		graphicsRootSignatureInfo.rootParameterInfos[1].baseRegister = 1;
		graphicsRootSignatureInfo.rootParameterInfos[1].registerSpace = 0;
		graphicsRootSignatureInfo.rootParameterInfos[1].numDescriptors = 1;
		graphicsRootSignatureInfo.rootParameterInfos[1].stages = STAGE_VERTEX;
		graphicsRootSignatureInfo.rootParameterInfos[1].type = DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		graphicsRootSignatureInfo.rootParameterInfos[1].updateFrequency = UPDATE_FREQUENCY_PER_FRAME;

		graphicsRootSignatureInfo.rootParameterInfos[2].binding = 0;
		graphicsRootSignatureInfo.rootParameterInfos[2].baseRegister = 0;
		graphicsRootSignatureInfo.rootParameterInfos[2].registerSpace = 0;
		graphicsRootSignatureInfo.rootParameterInfos[2].numDescriptors = 1;
		graphicsRootSignatureInfo.rootParameterInfos[2].stages = STAGE_PIXEL;
		graphicsRootSignatureInfo.rootParameterInfos[2].type = DESCRIPTOR_TYPE_TEXTURE;
		graphicsRootSignatureInfo.rootParameterInfos[2].updateFrequency = UPDATE_FREQUENCY_PER_NONE;

		graphicsRootSignatureInfo.rootParameterInfos[3].binding = 0;
		graphicsRootSignatureInfo.rootParameterInfos[3].baseRegister = 0;
		graphicsRootSignatureInfo.rootParameterInfos[3].registerSpace = 0;
		graphicsRootSignatureInfo.rootParameterInfos[3].numDescriptors = 1;
		graphicsRootSignatureInfo.rootParameterInfos[3].stages = STAGE_PIXEL;
		graphicsRootSignatureInfo.rootParameterInfos[3].type = DESCRIPTOR_TYPE_SAMPLER;
		graphicsRootSignatureInfo.rootParameterInfos[3].updateFrequency = UPDATE_FREQUENCY_PER_SAMPLER;

		graphicsRootSignatureInfo.numRootParameterInfos = 4;
		graphicsRootSignatureInfo.useInputLayout = true;
		CreateRootSignature(&gRenderer, &graphicsRootSignatureInfo, &gGraphicsRootSignature);

		PipelineInfo graphicsPipelineInfo{};
		graphicsPipelineInfo.type = PIPELINE_TYPE_GRAPHICS;
		graphicsPipelineInfo.topology = TOPOLOGY_TRIANGLE_LIST;
		graphicsPipelineInfo.rasInfo.cullMode = CULL_MODE_BACK;
		graphicsPipelineInfo.rasInfo.faceMode = FACE_CLOCKWISE;
		graphicsPipelineInfo.rasInfo.fillMode = FILL_MODE_SOLID;
		graphicsPipelineInfo.rasInfo.lineWidth = 1.0f;
		graphicsPipelineInfo.blendInfo.enableBlend = false;
		graphicsPipelineInfo.pVertexShader = &gVertexShader;
		graphicsPipelineInfo.pPixelShader = &gPixelShader;
		graphicsPipelineInfo.renderTargetFormat = gSwapChain.pRenderTargets[0].info.format;
		graphicsPipelineInfo.depthInfo.depthTestEnable = true;
		graphicsPipelineInfo.depthInfo.depthWriteEnable = true;
		graphicsPipelineInfo.depthInfo.depthFunction = DEPTH_FUNCTION_LESS;
		graphicsPipelineInfo.depthFormat = gDepthBuffer.info.format;
		graphicsPipelineInfo.pVertexInputInfo = &vertexInputInfo;
		graphicsPipelineInfo.pRootSignature = &gGraphicsRootSignature;
		CreatePipeline(&gRenderer, &graphicsPipelineInfo, &gGraphicsPipeline);

		for (uint32_t i = 0; i < gNumFrames; ++i)
		{
			CreateCommandBuffer(&gRenderer, QUEUE_TYPE_GRAPHICS, &gGraphicsCommandBuffers[i]);
			CreateSemaphore(&gRenderer, &gImageAvailableSemaphores[i]);
		}

		gVertexOffsets[EQUILATERAL_TRIANGLE] = arrlenu(gVertices);
		CreateEquilateralTriangle(&gVertices);

		gVertexOffsets[RIGHT_TRIANGLE] = arrlenu(gVertices);
		CreateRightTriangle(&gVertices);

		gVertexOffsets[QUAD] = arrlenu(gVertices);
		gIndexOffsets[QUAD] = arrlenu(gIndices);
		CreateQuad(&gVertices, &gIndices, &gIndexCounts[QUAD]);

		gVertexOffsets[CIRCLE] = arrlenu(gVertices);
		gIndexOffsets[CIRCLE] = arrlenu(gIndices);
		CreateCircle(&gVertices, &gIndices, &gIndexCounts[CIRCLE]);

		BufferInfo vbInfo{};
		vbInfo.size = arrlen(gVertices) * sizeof(Vertex);
		vbInfo.type = BUFFER_TYPE_VERTEX;
		vbInfo.usage = MEMORY_USAGE_GPU_ONLY;
		vbInfo.data = gVertices;
		vbInfo.initialState = RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
		CreateBuffer(&gRenderer, &vbInfo, &gVertexBuffer);

		BufferInfo ibInfo{};
		ibInfo.size = arrlen(gIndices) * sizeof(uint32_t);
		ibInfo.type = BUFFER_TYPE_INDEX;
		ibInfo.usage = MEMORY_USAGE_GPU_ONLY;
		ibInfo.data = gIndices;
		ibInfo.initialState = RESOURCE_STATE_INDEX_BUFFER;
		CreateBuffer(&gRenderer, &ibInfo, &gIndexBuffer);

		for (uint32_t i = 0; i < gNumFrames; ++i)
		{
			BufferInfo ubInfo{};
			ubInfo.size = sizeof(PerObjectUniformData);
			ubInfo.type = BUFFER_TYPE_UNIFORM;
			ubInfo.usage = MEMORY_USAGE_CPU_TO_GPU;
			ubInfo.data = nullptr;

			CreateBuffer(&gRenderer, &ubInfo, &gEqTriangleUniformBuffer[i]);
			CreateBuffer(&gRenderer, &ubInfo, &gRightTriangleUniformBuffer[i]);
			CreateBuffer(&gRenderer, &ubInfo, &gQuadUniformBuffer[i]);
			CreateBuffer(&gRenderer, &ubInfo, &gCircleUniformBuffer[i]);

			ubInfo.size = sizeof(PerFrameUniformData);
			CreateBuffer(&gRenderer, &ubInfo, &gPerFrameBuffer[i]);
		}

		TextureInfo texInfo{};
		texInfo.filename = "statue.dds";
		CreateTexture(&gRenderer, &texInfo, &gStatueTexture);

		SamplerInfo samplerInfo{};
		samplerInfo.magFilter = FILTER_LINEAR;
		samplerInfo.minFilter = FILTER_LINEAR;
		samplerInfo.u = ADDRESS_MODE_REPEAT;
		samplerInfo.v = ADDRESS_MODE_REPEAT;
		samplerInfo.w = ADDRESS_MODE_REPEAT;
		samplerInfo.mipMapMode = MIPMAP_MODE_LINEAR;
		samplerInfo.maxAnisotropy = 0.0f;
		samplerInfo.mipLoadBias = 0.0f;
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = 0.0f;
		CreateSampler(&gRenderer, &samplerInfo, &gStatueSampler);

		DescriptorSetInfo uniformSetInfo{};
		uniformSetInfo.pRootSignature = &gGraphicsRootSignature;
		uniformSetInfo.numSets = MAX_SHAPES * gNumFrames;
		uniformSetInfo.updateFrequency = UPDATE_FREQUENCY_PER_FRAME;
		CreateDescriptorSet(&gRenderer, &uniformSetInfo, &gDescriptorSetUniforms);

		DescriptorSetInfo texturesSetInfo{};
		texturesSetInfo.pRootSignature = &gGraphicsRootSignature;
		texturesSetInfo.numSets = 1;
		texturesSetInfo.updateFrequency = UPDATE_FREQUENCY_PER_NONE;
		CreateDescriptorSet(&gRenderer, &texturesSetInfo, &gDescriptorSetTextures);

		DescriptorSetInfo samplersSetInfo{};
		samplersSetInfo.pRootSignature = &gGraphicsRootSignature;
		samplersSetInfo.numSets = 1;
		samplersSetInfo.updateFrequency = UPDATE_FREQUENCY_PER_SAMPLER;
		CreateDescriptorSet(&gRenderer, &samplersSetInfo, &gDescriptorSetSamplers);

		for (uint32_t i = 0; i < gNumFrames; ++i)
		{
			UpdateDescriptorSetInfo updateUniformSetInfo[2]{};
			updateUniformSetInfo[0].binding = 0;
			updateUniformSetInfo[0].type = UPDATE_TYPE_UNIFORM_BUFFER;
			updateUniformSetInfo[0].pBuffer = &gPerFrameBuffer[i];
			updateUniformSetInfo[1].binding = 1;
			updateUniformSetInfo[1].type = UPDATE_TYPE_UNIFORM_BUFFER;
			updateUniformSetInfo[1].pBuffer = &gEqTriangleUniformBuffer[i];
			UpdateDescriptorSet(&gRenderer, &gDescriptorSetUniforms, i + EQUILATERAL_TRIANGLE * 2, 2, updateUniformSetInfo);

			updateUniformSetInfo[1].pBuffer = &gRightTriangleUniformBuffer[i];
			UpdateDescriptorSet(&gRenderer, &gDescriptorSetUniforms, i + RIGHT_TRIANGLE * 2, 2, updateUniformSetInfo);

			updateUniformSetInfo[1].pBuffer = &gQuadUniformBuffer[i];
			UpdateDescriptorSet(&gRenderer, &gDescriptorSetUniforms, i + QUAD * 2, 2, updateUniformSetInfo);

			updateUniformSetInfo[1].pBuffer = &gCircleUniformBuffer[i];
			UpdateDescriptorSet(&gRenderer, &gDescriptorSetUniforms, i + CIRCLE * 2, 2, updateUniformSetInfo);
		}

		UpdateDescriptorSetInfo updateImageSetInfo[1]{};
		updateImageSetInfo[0].binding = 0;
		updateImageSetInfo[0].type = UPDATE_TYPE_TEXTURE;
		updateImageSetInfo[0].pTexture = &gStatueTexture;
		UpdateDescriptorSet(&gRenderer, &gDescriptorSetTextures, 0, 1, updateImageSetInfo);

		UpdateDescriptorSetInfo updateSamplerSetInfo[1];
		updateSamplerSetInfo[0].binding = 0;
		updateSamplerSetInfo[0].type = UPDATE_TYPE_SAMPLER;
		updateSamplerSetInfo[0].pSampler = &gStatueSampler;
		UpdateDescriptorSet(&gRenderer, &gDescriptorSetSamplers, 0, 1, updateSamplerSetInfo);

		LookAt(&gCamera, vec3(1.0f, 0.0f, -7.5f), vec3(1.0f, 0.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f));

		gCamera.vFov = 45.0f;
		gCamera.nearP = 1.0f;
		gCamera.farP = 100.0f;

		gCubeRotation = mat4::MakeIdentity();

		UiInfo uiInfo{};
		uiInfo.pWindow = pWindow;
		uiInfo.pQueue = &gGraphicsQueue;
		uiInfo.numFrames = gNumFrames;
		uiInfo.renderTargetFormat = gSwapChain.info.format;
		uiInfo.depthFormat = gDepthBuffer.info.format;
		uiInfo.numImages = gSwapChain.numRenderTargets;
		InitUI(&gRenderer, &uiInfo);
	}

	void Exit() override
	{
		WaitQueueIdle(&gRenderer, &gGraphicsQueue);

		DestroyShape(&gVertices, &gIndices);
		
		DestroyUI(&gRenderer);
		DestroyDescriptorSet(&gDescriptorSetSamplers);
		DestroyDescriptorSet(&gDescriptorSetTextures);
		DestroyDescriptorSet(&gDescriptorSetUniforms);

		DestroySampler(&gRenderer, &gStatueSampler);
		DestroyTexture(&gRenderer, &gStatueTexture);

		for (uint32_t i = 0; i < gNumFrames; ++i)
		{
			DestroyBuffer(&gRenderer, &gPerFrameBuffer[i]);
			DestroyBuffer(&gRenderer, &gCircleUniformBuffer[i]);
			DestroyBuffer(&gRenderer, &gQuadUniformBuffer[i]);
			DestroyBuffer(&gRenderer, &gRightTriangleUniformBuffer[i]);
			DestroyBuffer(&gRenderer, &gEqTriangleUniformBuffer[i]);
			DestroySemaphore(&gRenderer, &gImageAvailableSemaphores[i]);
			DestroyCommandBuffer(&gRenderer, &gGraphicsCommandBuffers[i]);
		}

		DestroyPipeline(&gRenderer, &gGraphicsPipeline);

		DestroyRootSignature(&gRenderer, &gGraphicsRootSignature);

		DestroyBuffer(&gRenderer, &gIndexBuffer);
		DestroyBuffer(&gRenderer, &gVertexBuffer);

		DestroyShader(&gRenderer, &gPixelShader);
		DestroyShader(&gRenderer, &gVertexShader);

		DestroyRenderTarget(&gRenderer, &gDepthBuffer);

		DestroySwapChain(&gRenderer, &gSwapChain);

		DestroyQueue(&gRenderer, &gGraphicsQueue);

		DestroyRenderer(&gRenderer);

		ExitSE();
	}

	void UserInput(float deltaTime) override
	{
		static float moveSpeed = 5.0f;
		static float turnSpeed = 0.25f;
		static float turnSpeed2 = 25.0f;
		static vec2 lastMousePosition;


		if (CheckKeyDown('W'))
			MoveForward(&gCamera, moveSpeed * deltaTime);
		if (CheckKeyDown('S'))
			MoveBackward(&gCamera, moveSpeed * deltaTime);
		if (CheckKeyDown('A'))
			MoveLeft(&gCamera, moveSpeed * deltaTime);
		if (CheckKeyDown('D'))
			MoveRight(&gCamera, moveSpeed * deltaTime);
		if (CheckKeyDown('Q'))
			MoveUp(&gCamera, moveSpeed * deltaTime);
		if (CheckKeyDown('E'))
			MoveDown(&gCamera, moveSpeed * deltaTime);
		if (CheckKeyDown(VK_SPACE))
			LookAt(&gCamera, vec3(1.0f, 0.0f, -7.5f), vec3(1.0f, 0.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f));
		if (CheckKeyDown('L'))
			RotateCamera(&gCamera, quat::MakeRotation(turnSpeed2 * deltaTime, vec3(0.0f, 1.0f, 0.0f)));
		if (CheckKeyDown('J'))
			RotateCamera(&gCamera, quat::MakeRotation(-turnSpeed2 * deltaTime, vec3(0.0f, 1.0f, 0.0f)));
		if (CheckKeyDown('I'))
			RotateCamera(&gCamera, quat::MakeRotation(turnSpeed2 * deltaTime, gCamera.right));
		if (CheckKeyDown('K'))
			RotateCamera(&gCamera, quat::MakeRotation(-turnSpeed2 * deltaTime, gCamera.right));

		POINT currMousePos{};
		GetCursorPos(&currMousePos);
		vec2 currentMousePosition{ (float)currMousePos.x, (float)currMousePos.y };
		vec2 mousePositionDiff{ currentMousePosition - lastMousePosition };
		if ((CheckKeyDown(VK_LBUTTON) && Length(mousePositionDiff) < 20.0f))
		{
			RotateCamera(&gCamera, quat::MakeRotation(turnSpeed * mousePositionDiff.GetX(), vec3(0.0f, 1.0f, 0.0f)));
			RotateCamera(&gCamera, quat::MakeRotation(turnSpeed * mousePositionDiff.GetY(), gCamera.right));
		}

		lastMousePosition = currentMousePosition;

	}

	void Resize() override
	{
		WaitQueueIdle(&gRenderer, &gGraphicsQueue);

		DestroySwapChain(&gRenderer, &gSwapChain);

		uint32_t newWidth = GetWidth(pWindow);
		uint32_t newHeight = GetHeight(pWindow);

		gSwapChain.info.width = newWidth;
		gSwapChain.info.height = newHeight;

		CreateSwapChain(&gRenderer, &gSwapChain.info, &gSwapChain);

		DestroyRenderTarget(&gRenderer, &gDepthBuffer);

		gDepthBuffer.info.width = newWidth;
		gDepthBuffer.info.height = newHeight;
		CreateRenderTarget(&gRenderer, &gDepthBuffer.info, &gDepthBuffer);
	}

	void Update(float deltaTime) override
	{
		gCamera.aspectRatio = (float)GetWidth(pWindow) / GetHeight(pWindow);
		UpdateViewMatrix(&gCamera);
		UpdatePerspectiveProjectionMatrix(&gCamera);

		mat4 model = mat4::Scale(1.0f, 1.0f, 1.0f) * mat4::Translate(0.0f, 0.0f, 0.0f);
		gShapesUniformData[EQUILATERAL_TRIANGLE].model = Transpose(model);

		model = mat4::Scale(1.0f, 1.0f, 1.0f) * mat4::Translate(2.0f, 0.0f, 0.0f);
		gShapesUniformData[RIGHT_TRIANGLE].model = Transpose(model);

		model = mat4::Scale(1.0f, 1.0f, 1.0f) * mat4::Translate(0.0f, 2.0f, 0.0f);
		gShapesUniformData[QUAD].model = Transpose(model);

		model = mat4::Scale(1.0f, 1.0f, 1.0f) * mat4::Translate(2.0f, 2.0f, 0.0f);
		gShapesUniformData[CIRCLE].model = Transpose(model);

		gPerFrameUniformData.view = Transpose(gCamera.viewMat);
		gPerFrameUniformData.projection = Transpose(gCamera.perspectiveProjMat);
	}

	void Draw() override
	{
		CommandBuffer* pCommandBuffer = &gGraphicsCommandBuffers[gCurrentFrame];
		WaitForFence(&gRenderer, &pCommandBuffer->fence);

		void* data = nullptr;
		MapMemory(&gRenderer, &gEqTriangleUniformBuffer[gCurrentFrame], &data);
		memcpy(data, &gShapesUniformData[EQUILATERAL_TRIANGLE], sizeof(PerObjectUniformData));
		UnmapMemory(&gRenderer, &gEqTriangleUniformBuffer[gCurrentFrame]);

		MapMemory(&gRenderer, &gRightTriangleUniformBuffer[gCurrentFrame], &data);
		memcpy(data, &gShapesUniformData[RIGHT_TRIANGLE], sizeof(PerObjectUniformData));
		UnmapMemory(&gRenderer, &gRightTriangleUniformBuffer[gCurrentFrame]);

		MapMemory(&gRenderer, &gQuadUniformBuffer[gCurrentFrame], &data);
		memcpy(data, &gShapesUniformData[QUAD], sizeof(PerObjectUniformData));
		UnmapMemory(&gRenderer, &gQuadUniformBuffer[gCurrentFrame]);

		MapMemory(&gRenderer, &gCircleUniformBuffer[gCurrentFrame], &data);
		memcpy(data, &gShapesUniformData[CIRCLE], sizeof(PerObjectUniformData));
		UnmapMemory(&gRenderer, &gCircleUniformBuffer[gCurrentFrame]);

		MapMemory(&gRenderer, &gPerFrameBuffer[gCurrentFrame], &data);
		memcpy(data, &gPerFrameUniformData, sizeof(PerFrameUniformData));
		UnmapMemory(&gRenderer, &gPerFrameBuffer[gCurrentFrame]);

		uint32_t imageIndex = 0;
		AcquireNextImage(&gRenderer, &gSwapChain, &gImageAvailableSemaphores[gCurrentFrame], &imageIndex);

		RenderTarget* pRenderTarget = &gSwapChain.pRenderTargets[imageIndex];

		ResetCommandBuffer(&gRenderer, pCommandBuffer);

		BeginCommandBuffer(pCommandBuffer);

		BarrierInfo barrierInfo{};
		barrierInfo.type = BARRIER_TYPE_RENDER_TARGET;
		barrierInfo.pRenderTarget = pRenderTarget;
		barrierInfo.currentState = RESOURCE_STATE_PRESENT;
		barrierInfo.newState = RESOURCE_STATE_RENDER_TARGET;
		ResourceBarrier(pCommandBuffer, 1, &barrierInfo);

		BindRenderTargetInfo renderTargetInfo{};
		renderTargetInfo.pRenderTarget = pRenderTarget;
		renderTargetInfo.renderTargetLoadOp = LOAD_OP_CLEAR;
		renderTargetInfo.renderTargetStoreOp = STORE_OP_STORE;
		renderTargetInfo.pDepthTarget = &gDepthBuffer;
		renderTargetInfo.depthTargetLoadOp = LOAD_OP_CLEAR;
		renderTargetInfo.depthTargetStoreOp = STORE_OP_DONT_CARE;
		BindRenderTarget(pCommandBuffer, &renderTargetInfo);

		ViewportInfo viewportInfo{};
		viewportInfo.x = 0.0f;
		viewportInfo.y = 0.0f;
		viewportInfo.width = pRenderTarget->info.width;
		viewportInfo.height = pRenderTarget->info.height;
		viewportInfo.minDepth = 0.0f;
		viewportInfo.maxDepth = 1.0f;
		SetViewport(pCommandBuffer, &viewportInfo);

		ScissorInfo scissorInfo{};
		scissorInfo.x = 0.0f;
		scissorInfo.y = 0.0f;
		scissorInfo.width = pRenderTarget->info.width;
		scissorInfo.height = pRenderTarget->info.height;
		SetScissor(pCommandBuffer, &scissorInfo);


		BindPipeline(pCommandBuffer, &gGraphicsPipeline);

		BindVertexBuffer(pCommandBuffer, sizeof(Vertex), 0, 0, &gVertexBuffer);

		BindDescriptorSet(pCommandBuffer, 0, 0, &gDescriptorSetTextures);
		BindDescriptorSet(pCommandBuffer, 0, 2, &gDescriptorSetSamplers);

		BindDescriptorSet(pCommandBuffer, gCurrentFrame + EQUILATERAL_TRIANGLE * gNumFrames, 1, &gDescriptorSetUniforms);
		DrawInstanced(pCommandBuffer, 3, 1, gVertexOffsets[EQUILATERAL_TRIANGLE], 0);

		BindDescriptorSet(pCommandBuffer, gCurrentFrame + RIGHT_TRIANGLE * gNumFrames, 1, &gDescriptorSetUniforms);
		DrawInstanced(pCommandBuffer, 3, 1, gVertexOffsets[RIGHT_TRIANGLE], 0);

		BindIndexBuffer(pCommandBuffer, 0, INDEX_TYPE_UINT32, &gIndexBuffer);
		BindDescriptorSet(pCommandBuffer, gCurrentFrame + QUAD * gNumFrames, 1, &gDescriptorSetUniforms);
		DrawIndexedInstanced(pCommandBuffer, gIndexCounts[QUAD], 1, gIndexOffsets[QUAD], gVertexOffsets[QUAD], 0);

		BindDescriptorSet(pCommandBuffer, gCurrentFrame + CIRCLE * gNumFrames, 1, &gDescriptorSetUniforms);
		DrawIndexedInstanced(pCommandBuffer, gIndexCounts[CIRCLE], 1, gIndexOffsets[CIRCLE], gVertexOffsets[CIRCLE], 0);

		RenderUI(pCommandBuffer);

		BindRenderTarget(pCommandBuffer, nullptr);

		barrierInfo.type = BARRIER_TYPE_RENDER_TARGET;
		barrierInfo.pRenderTarget = pRenderTarget;
		barrierInfo.currentState = RESOURCE_STATE_RENDER_TARGET;
		barrierInfo.newState = RESOURCE_STATE_PRESENT;
		ResourceBarrier(pCommandBuffer, 1, &barrierInfo);

		EndCommandBuffer(pCommandBuffer);

		Semaphore submitWaitSempahores[] = { gImageAvailableSemaphores[gCurrentFrame] };
		Semaphore submitSignalSemaphores[] = { pCommandBuffer->semaphore };
		Fence submitFence[] = { pCommandBuffer->fence };

		QueueSubmitInfo submitInfo{};
		submitInfo.pQueue = &gGraphicsQueue;
		submitInfo.numWaitSemaphores = 1;
		submitInfo.waitSemaphores = submitWaitSempahores;
		submitInfo.numSignalSemaphores = 1;
		submitInfo.signalSemaphores = submitSignalSemaphores;
		submitInfo.pFence = submitFence;
		submitInfo.pCommandBuffer = pCommandBuffer;
		QueueSubmit(&submitInfo);

		Semaphore presentWaitSemaphores[] = { pCommandBuffer->semaphore };

		PresentInfo presentInfo{};
		presentInfo.pQueue = &gGraphicsQueue;
		presentInfo.numWaitSemaphores = 1;
		presentInfo.waitSemaphores = presentWaitSemaphores;
		presentInfo.pSwapChain = &gSwapChain;
		presentInfo.imageIndex = imageIndex;
		QueuePresent(&presentInfo);

		gCurrentFrame = (gCurrentFrame + 1) % gNumFrames;
	}
};

int main()
{
	Test test;
	test.appName = "Test";
	return WindowsMain(&test);
};
