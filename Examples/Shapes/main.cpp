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

Shader gShapesVertexShader;
Shader gShapesPixelShader;
RootSignature gGraphicsRootSignature;
Pipeline gShapesPipeline;
Shader gSkyboxVertexShader;
Shader gSkyboxPixelShader;
Pipeline gSkyboxPipeline;

const uint32_t gNumFrames = 2;
Semaphore gImageAvailableSemaphores[gNumFrames];
CommandBuffer gGraphicsCommandBuffers[gNumFrames];

Buffer gVertexBuffer;
Buffer gIndexBuffer;

Texture gStatueTexture;
Texture gYokohamaTexture;
Texture gSkyboxTexture;
Sampler gSampler;

uint32_t gCurrentFrame = 0;

DescriptorSet gDescriptorSetPerFrame;
DescriptorSet gDescriptorSetPerNone;

Camera gCamera;

Vertex* gVertices = nullptr;
uint32_t* gIndices = nullptr;

struct PerObjectUniformData
{
	mat4 model;
	bool isShape2D;
};

struct PerFrameUniformData
{
	mat4 view;
	mat4 projection;
};

PerFrameUniformData gPerFrameUniformData;
Buffer gPerFrameBuffer[gNumFrames];

enum
{
	EQUILATERAL_TRIANGLE,
	RIGHT_TRIANGLE,
	QUAD,
	CIRCLE,
	BOX,
	SQUARE_PYRAMID,
	TRIANGULAR_PYRAMID,
	SPHERE,
	HEMI_SPHERE,
	HEMI_SPHERE_BASE,
	CYLINDER,
	CYLINDER_TOP_BASE,
	CYLINDER_BOTTOM_BASE,
	CYLINDER_BASE,
	CONE,
	CONE_BASE,
	TORUS,
	MAX_SHAPES
};

PerObjectUniformData gShapesUniformData[MAX_SHAPES];
Buffer gShapesUniformBuffers[MAX_SHAPES * gNumFrames];

PerObjectUniformData gSkyBoxUniformData;
Buffer gSkyboxUniformBuffer[gNumFrames];

PerFrameUniformData gSkyboxPerFrameUniformData;
Buffer gSkyboxPerFrameBuffer[gNumFrames];

uint32_t gVertexOffsets[MAX_SHAPES];
uint32_t gIndexOffsets[MAX_SHAPES];
uint32_t gIndexCounts[MAX_SHAPES];

mat4 gCubeRotation;
float gAngle = 0.0f;

class Shapes : public App
{
public:
	void Init() override
	{
		InitSE();

		InitRenderer(&gRenderer, "Shapes");

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
		vertexShaderInfo.glslFilename = "CompiledShaders/GLSL/shapesVS.spv";
		vertexShaderInfo.hlslFilename = "CompiledShaders/HLSL/shapesVS.cso";
		vertexShaderInfo.type = SHADER_TYPE_VERTEX;
		CreateShader(&gRenderer, &vertexShaderInfo, &gShapesVertexShader);

		ShaderInfo pixelShaderInfo{};
		pixelShaderInfo.glslFilename = "CompiledShaders/GLSL/shapesPS.spv";
		pixelShaderInfo.hlslFilename = "CompiledShaders/HLSL/shapesPS.cso";
		pixelShaderInfo.type = SHADER_TYPE_PIXEL;
		CreateShader(&gRenderer, &pixelShaderInfo, &gShapesPixelShader);

		vertexShaderInfo.glslFilename = "CompiledShaders/GLSL/skyboxVS.spv";
		vertexShaderInfo.hlslFilename = "CompiledShaders/HLSL/skyboxVS.cso";
		vertexShaderInfo.type = SHADER_TYPE_VERTEX;
		CreateShader(&gRenderer, &vertexShaderInfo, &gSkyboxVertexShader);

		pixelShaderInfo.glslFilename = "CompiledShaders/GLSL/skyboxPS.spv";
		pixelShaderInfo.hlslFilename = "CompiledShaders/HLSL/skyboxPS.cso";
		pixelShaderInfo.type = SHADER_TYPE_PIXEL;
		CreateShader(&gRenderer, &pixelShaderInfo, &gSkyboxPixelShader);

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

		RootParameterInfo rootParameterInfos[5]{};
		rootParameterInfos[0].binding = 0;
		rootParameterInfos[0].baseRegister = 0;
		rootParameterInfos[0].registerSpace = 0;
		rootParameterInfos[0].numDescriptors = 1;
		rootParameterInfos[0].stages = STAGE_VERTEX;
		rootParameterInfos[0].type = DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		rootParameterInfos[0].updateFrequency = UPDATE_FREQUENCY_PER_FRAME;

		rootParameterInfos[1].binding = 1;
		rootParameterInfos[1].baseRegister = 1;
		rootParameterInfos[1].registerSpace = 0;
		rootParameterInfos[1].numDescriptors = 1;
		rootParameterInfos[1].stages = STAGE_VERTEX | STAGE_PIXEL;
		rootParameterInfos[1].type = DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		rootParameterInfos[1].updateFrequency = UPDATE_FREQUENCY_PER_FRAME;

		rootParameterInfos[2].binding = 0;
		rootParameterInfos[2].baseRegister = 0;
		rootParameterInfos[2].registerSpace = 0;
		rootParameterInfos[2].numDescriptors = 1;
		rootParameterInfos[2].stages = STAGE_PIXEL;
		rootParameterInfos[2].type = DESCRIPTOR_TYPE_TEXTURE;
		rootParameterInfos[2].updateFrequency = UPDATE_FREQUENCY_PER_NONE;

		rootParameterInfos[3].binding = 1;
		rootParameterInfos[3].baseRegister = 1;
		rootParameterInfos[3].registerSpace = 0;
		rootParameterInfos[3].numDescriptors = 1;
		rootParameterInfos[3].stages = STAGE_PIXEL;
		rootParameterInfos[3].type = DESCRIPTOR_TYPE_TEXTURE;
		rootParameterInfos[3].updateFrequency = UPDATE_FREQUENCY_PER_NONE;

		rootParameterInfos[4].binding = 2;
		rootParameterInfos[4].baseRegister = 0;
		rootParameterInfos[4].registerSpace = 0;
		rootParameterInfos[4].numDescriptors = 1;
		rootParameterInfos[4].stages = STAGE_PIXEL;
		rootParameterInfos[4].type = DESCRIPTOR_TYPE_SAMPLER;
		rootParameterInfos[4].updateFrequency = UPDATE_FREQUENCY_PER_NONE;

		RootSignatureInfo graphicsRootSignatureInfo{};
		graphicsRootSignatureInfo.pRootParameterInfos = rootParameterInfos;
		graphicsRootSignatureInfo.numRootParameterInfos = 5;
		graphicsRootSignatureInfo.useRootConstants = false;
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
		graphicsPipelineInfo.pVertexShader = &gShapesVertexShader;
		graphicsPipelineInfo.pPixelShader = &gShapesPixelShader;
		graphicsPipelineInfo.renderTargetFormat = gSwapChain.pRenderTargets[0].info.format;
		graphicsPipelineInfo.depthInfo.depthTestEnable = true;
		graphicsPipelineInfo.depthInfo.depthWriteEnable = true;
		graphicsPipelineInfo.depthInfo.depthFunction = DEPTH_FUNCTION_LESS;
		graphicsPipelineInfo.depthFormat = gDepthBuffer.info.format;
		graphicsPipelineInfo.pVertexInputInfo = &vertexInputInfo;
		graphicsPipelineInfo.pRootSignature = &gGraphicsRootSignature;
		CreatePipeline(&gRenderer, &graphicsPipelineInfo, &gShapesPipeline);

		graphicsPipelineInfo.rasInfo.cullMode = CULL_MODE_NONE;
		graphicsPipelineInfo.pVertexShader = &gSkyboxVertexShader;
		graphicsPipelineInfo.pPixelShader = &gSkyboxPixelShader;
		graphicsPipelineInfo.depthInfo.depthFunction = DEPTH_FUNCTION_LESS_OR_EQUAL;
		CreatePipeline(&gRenderer, &graphicsPipelineInfo, &gSkyboxPipeline);

		for (uint32_t i = 0; i < gNumFrames; ++i)
		{
			CreateCommandBuffer(&gRenderer, QUEUE_TYPE_GRAPHICS, &gGraphicsCommandBuffers[i]);
			CreateSemaphore(&gRenderer, &gImageAvailableSemaphores[i]);
		}

		gVertexOffsets[EQUILATERAL_TRIANGLE] = arrlenu(gVertices);
		gIndexOffsets[EQUILATERAL_TRIANGLE] = arrlenu(gIndices);
		CreateEquilateralTriangle(&gVertices, &gIndices, &gIndexCounts[EQUILATERAL_TRIANGLE]);

		gVertexOffsets[RIGHT_TRIANGLE] = arrlenu(gVertices);
		gIndexOffsets[RIGHT_TRIANGLE] = arrlenu(gIndices);
		CreateRightTriangle(&gVertices, &gIndices, &gIndexCounts[RIGHT_TRIANGLE]);

		gVertexOffsets[QUAD] = arrlenu(gVertices);
		gIndexOffsets[QUAD] = arrlenu(gIndices);
		CreateQuad(&gVertices, &gIndices, &gIndexCounts[QUAD]);

		gVertexOffsets[CIRCLE] = arrlenu(gVertices);
		gIndexOffsets[CIRCLE] = arrlenu(gIndices);
		CreateCircle(&gVertices, &gIndices, &gIndexCounts[CIRCLE]);

		gVertexOffsets[BOX] = arrlenu(gVertices);
		gIndexOffsets[BOX] = arrlenu(gIndices);
		CreateBox(&gVertices, &gIndices, &gIndexCounts[BOX]);

		gVertexOffsets[SQUARE_PYRAMID] = arrlenu(gVertices);
		gIndexOffsets[SQUARE_PYRAMID] = arrlenu(gIndices);
		CreateSquarePyramid(&gVertices, &gIndices, &gIndexCounts[SQUARE_PYRAMID]);

		gVertexOffsets[TRIANGULAR_PYRAMID] = arrlenu(gVertices);
		gIndexOffsets[TRIANGULAR_PYRAMID] = arrlenu(gIndices);
		CreateTriangularPyramid(&gVertices, &gIndices, &gIndexCounts[TRIANGULAR_PYRAMID]);

		gVertexOffsets[SPHERE] = arrlenu(gVertices);
		gIndexOffsets[SPHERE] = arrlenu(gIndices);
		CreateSphere(&gVertices, &gIndices, &gIndexCounts[SPHERE]);

		gVertexOffsets[HEMI_SPHERE] = arrlenu(gVertices);
		gIndexOffsets[HEMI_SPHERE] = arrlenu(gIndices);
		CreateHemiSphere(&gVertices, &gIndices, &gIndexCounts[HEMI_SPHERE], false);

		gVertexOffsets[HEMI_SPHERE_BASE] = arrlenu(gVertices);
		gIndexOffsets[HEMI_SPHERE_BASE] = arrlenu(gIndices);
		CreateHemiSphere(&gVertices, &gIndices, &gIndexCounts[HEMI_SPHERE_BASE], true);

		gVertexOffsets[CYLINDER] = arrlenu(gVertices);
		gIndexOffsets[CYLINDER] = arrlenu(gIndices);
		CreateCylinder(&gVertices, &gIndices, &gIndexCounts[CYLINDER], false, false);

		gVertexOffsets[CYLINDER_TOP_BASE] = arrlenu(gVertices);
		gIndexOffsets[CYLINDER_TOP_BASE] = arrlenu(gIndices);
		CreateCylinder(&gVertices, &gIndices, &gIndexCounts[CYLINDER_TOP_BASE], true, false);

		gVertexOffsets[CYLINDER_BOTTOM_BASE] = arrlenu(gVertices);
		gIndexOffsets[CYLINDER_BOTTOM_BASE] = arrlenu(gIndices);
		CreateCylinder(&gVertices, &gIndices, &gIndexCounts[CYLINDER_BOTTOM_BASE], false, true);

		gVertexOffsets[CYLINDER_BASE] = arrlenu(gVertices);
		gIndexOffsets[CYLINDER_BASE] = arrlenu(gIndices);
		CreateCylinder(&gVertices, &gIndices, &gIndexCounts[CYLINDER_BASE], true, true);

		gVertexOffsets[CONE] = arrlenu(gVertices);
		gIndexOffsets[CONE] = arrlenu(gIndices);
		CreateCone(&gVertices, &gIndices, &gIndexCounts[CONE], false);

		gVertexOffsets[CONE_BASE] = arrlenu(gVertices);
		gIndexOffsets[CONE_BASE] = arrlenu(gIndices);
		CreateCone(&gVertices, &gIndices, &gIndexCounts[CONE_BASE], true);

		gVertexOffsets[TORUS] = arrlenu(gVertices);
		gIndexOffsets[TORUS] = arrlenu(gIndices);
		CreateTorus(&gVertices, &gIndices, &gIndexCounts[TORUS], 2.0f, 1.0f);

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
			ubInfo.size = sizeof(PerFrameUniformData);
			ubInfo.type = BUFFER_TYPE_UNIFORM;
			ubInfo.usage = MEMORY_USAGE_CPU_TO_GPU;
			ubInfo.data = nullptr;

			CreateBuffer(&gRenderer, &ubInfo, &gPerFrameBuffer[i]);
			CreateBuffer(&gRenderer, &ubInfo, &gSkyboxPerFrameBuffer[i]);

			ubInfo.size = sizeof(PerObjectUniformData);
			CreateBuffer(&gRenderer, &ubInfo, &gSkyboxUniformBuffer[i]);
		}

		for (uint32_t i = 0; i < MAX_SHAPES * gNumFrames; ++i)
		{
			BufferInfo ubInfo{};
			ubInfo.size = sizeof(PerObjectUniformData);
			ubInfo.type = BUFFER_TYPE_UNIFORM;
			ubInfo.usage = MEMORY_USAGE_CPU_TO_GPU;
			ubInfo.data = nullptr;

			CreateBuffer(&gRenderer, &ubInfo, &gShapesUniformBuffers[i]);
		}

		TextureInfo texInfo{};
		texInfo.filename = "Textures/statue.dds";
		CreateTexture(&gRenderer, &texInfo, &gStatueTexture);

		texInfo.filename = "Textures/yokohama.dds";
		CreateTexture(&gRenderer, &texInfo, &gYokohamaTexture);

		texInfo.filename = "Textures/skybox.dds";
		CreateTexture(&gRenderer, &texInfo, &gSkyboxTexture);

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
		CreateSampler(&gRenderer, &samplerInfo, &gSampler);

		DescriptorSetInfo uniformSetInfo{};
		uniformSetInfo.pRootSignature = &gGraphicsRootSignature;
		uniformSetInfo.numSets = MAX_SHAPES * gNumFrames + 2;
		uniformSetInfo.updateFrequency = UPDATE_FREQUENCY_PER_FRAME;
		CreateDescriptorSet(&gRenderer, &uniformSetInfo, &gDescriptorSetPerFrame);

		DescriptorSetInfo texturesSetInfo{};
		texturesSetInfo.pRootSignature = &gGraphicsRootSignature;
		texturesSetInfo.numSets = 2;
		texturesSetInfo.updateFrequency = UPDATE_FREQUENCY_PER_NONE;
		CreateDescriptorSet(&gRenderer, &texturesSetInfo, &gDescriptorSetPerNone);

		for (uint32_t i = 0; i < MAX_SHAPES * gNumFrames; ++i)
		{
			UpdateDescriptorSetInfo updatePerFrame[2]{};
			updatePerFrame[0].binding = 0;
			updatePerFrame[0].type = UPDATE_TYPE_UNIFORM_BUFFER;
			updatePerFrame[0].pBuffer = &gPerFrameBuffer[i / MAX_SHAPES];

			updatePerFrame[1].binding = 1;
			updatePerFrame[1].type = UPDATE_TYPE_UNIFORM_BUFFER;
			updatePerFrame[1].pBuffer = &gShapesUniformBuffers[i];

			UpdateDescriptorSet(&gRenderer, &gDescriptorSetPerFrame, i, 2, updatePerFrame);
		}

		for (uint32_t i = 0; i < gNumFrames; ++i)
		{
			UpdateDescriptorSetInfo updatePerFrame[2]{};
			updatePerFrame[0].binding = 0;
			updatePerFrame[0].type = UPDATE_TYPE_UNIFORM_BUFFER;
			updatePerFrame[0].pBuffer = &gSkyboxPerFrameBuffer[i];

			updatePerFrame[1].binding = 1;
			updatePerFrame[1].type = UPDATE_TYPE_UNIFORM_BUFFER;
			updatePerFrame[1].pBuffer = &gSkyboxUniformBuffer[i];

			UpdateDescriptorSet(&gRenderer, &gDescriptorSetPerFrame, i + MAX_SHAPES * gNumFrames, 2, updatePerFrame);
		}

		UpdateDescriptorSetInfo updateImageSetInfo[3]{};
		updateImageSetInfo[0].binding = 0;
		updateImageSetInfo[0].type = UPDATE_TYPE_TEXTURE;
		updateImageSetInfo[0].pTexture = &gStatueTexture;
		updateImageSetInfo[1].binding = 1;
		updateImageSetInfo[1].type = UPDATE_TYPE_TEXTURE;
		updateImageSetInfo[1].pTexture = &gYokohamaTexture;
		updateImageSetInfo[2].binding = 2;
		updateImageSetInfo[2].type = UPDATE_TYPE_SAMPLER;
		updateImageSetInfo[2].pSampler = &gSampler;
		UpdateDescriptorSet(&gRenderer, &gDescriptorSetPerNone, 0, 3, updateImageSetInfo);

		updateImageSetInfo[1].pTexture = &gSkyboxTexture;
		UpdateDescriptorSet(&gRenderer, &gDescriptorSetPerNone, 1, 3, updateImageSetInfo);

		LookAt(&gCamera, vec3(4.0f, 0.0f, -12.5f), vec3(4.0f, 0.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f));

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
		DestroyDescriptorSet(&gDescriptorSetPerNone);
		DestroyDescriptorSet(&gDescriptorSetPerFrame);

		DestroySampler(&gRenderer, &gSampler);
		DestroyTexture(&gRenderer, &gStatueTexture);
		DestroyTexture(&gRenderer, &gYokohamaTexture);
		DestroyTexture(&gRenderer, &gSkyboxTexture);

		for (uint32_t i = 0; i < gNumFrames; ++i)
		{
			DestroyBuffer(&gRenderer, &gPerFrameBuffer[i]);
			DestroyBuffer(&gRenderer, &gSkyboxPerFrameBuffer[i]);
			DestroyBuffer(&gRenderer, &gSkyboxUniformBuffer[i]);
			DestroySemaphore(&gRenderer, &gImageAvailableSemaphores[i]);
			DestroyCommandBuffer(&gRenderer, &gGraphicsCommandBuffers[i]);
		}

		for (uint32_t i = 0; i < MAX_SHAPES * gNumFrames; ++i)
		{
			DestroyBuffer(&gRenderer, &gShapesUniformBuffers[i]);
		}

		DestroyPipeline(&gRenderer, &gSkyboxPipeline);
		DestroyPipeline(&gRenderer, &gShapesPipeline);

		DestroyRootSignature(&gRenderer, &gGraphicsRootSignature);

		DestroyBuffer(&gRenderer, &gIndexBuffer);
		DestroyBuffer(&gRenderer, &gVertexBuffer);

		DestroyShader(&gRenderer, &gSkyboxVertexShader);
		DestroyShader(&gRenderer, &gSkyboxPixelShader);
		DestroyShader(&gRenderer, &gShapesPixelShader);
		DestroyShader(&gRenderer, &gShapesVertexShader);

		DestroyRenderTarget(&gRenderer, &gDepthBuffer);

		DestroySwapChain(&gRenderer, &gSwapChain);

		DestroyQueue(&gRenderer, &gGraphicsQueue);

		DestroyRenderer(&gRenderer);

		ExitSE();
	}

	void UserInput(float deltaTime) override
	{
		static float moveSpeed = 10.0f;
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
			LookAt(&gCamera, vec3(4.0f, 0.0f, -12.5f), vec3(4.0f, 0.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f));
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

		mat4 model = mat4::Scale(1.0f, 1.0f, 1.0f) * mat4::Translate(0.0f, 6.0f, 0.0f);
		gShapesUniformData[EQUILATERAL_TRIANGLE].model = Transpose(model);
		gShapesUniformData[EQUILATERAL_TRIANGLE].isShape2D = true;

		model = mat4::Scale(1.0f, 1.0f, 1.0f) * mat4::Translate(2.0f, 6.0f, 0.0f);
		gShapesUniformData[RIGHT_TRIANGLE].model = Transpose(model);
		gShapesUniformData[RIGHT_TRIANGLE].isShape2D = true;

		model = mat4::Scale(1.0f, 1.0f, 1.0f) * mat4::Translate(4.0f, 6.0f, 0.0f);
		gShapesUniformData[QUAD].model = Transpose(model);
		gShapesUniformData[QUAD].isShape2D = true;

		model = mat4::Scale(1.0f, 1.0f, 1.0f) * mat4::Translate(6.0f, 6.0f, 0.0f);
		gShapesUniformData[CIRCLE].model = Transpose(model);
		gShapesUniformData[CIRCLE].isShape2D = true;

		model = mat4::Scale(1.0f, 1.0f, 1.0f) * mat4::Translate(0.0f, 4.0f, 0.0f);
		gShapesUniformData[BOX].model = Transpose(model);
		gShapesUniformData[BOX].isShape2D = false;

		model = mat4::Scale(1.0f, 1.0f, 1.0f) * mat4::Translate(2.0f, 4.0f, 0.0f);
		gShapesUniformData[SQUARE_PYRAMID].model = Transpose(model);
		gShapesUniformData[SQUARE_PYRAMID].isShape2D = false;

		model = mat4::Scale(1.0f, 1.0f, 1.0f) * mat4::Translate(4.0f, 4.0f, 0.0f);
		gShapesUniformData[TRIANGULAR_PYRAMID].model = Transpose(model);
		gShapesUniformData[TRIANGULAR_PYRAMID].isShape2D = false;

		model = mat4::Scale(1.0f, 1.0f, 1.0f) * mat4::Translate(0.0f, 2.0f, 0.0f);
		gShapesUniformData[SPHERE].model = Transpose(model);
		gShapesUniformData[SPHERE].isShape2D = false;

		model = mat4::Scale(1.0f, 1.0f, 1.0f) * mat4::Translate(3.0f, 2.0f, 0.0f);
		gShapesUniformData[HEMI_SPHERE].model = Transpose(model);
		gShapesUniformData[HEMI_SPHERE].isShape2D = false;

		model = mat4::Scale(1.0f, 1.0f, 1.0f) * mat4::Translate(6.0f, 2.0f, 0.0f);
		gShapesUniformData[HEMI_SPHERE_BASE].model = Transpose(model);
		gShapesUniformData[HEMI_SPHERE_BASE].isShape2D = false;

		model = mat4::Scale(1.0f, 1.0f, 1.0f) * mat4::Translate(0.0f, 0.0f, 0.0f);
		gShapesUniformData[CYLINDER].model = Transpose(model);
		gShapesUniformData[CYLINDER].isShape2D = false;

		model = mat4::Scale(1.0f, 1.0f, 1.0f) * mat4::Translate(3.0f, 0.0f, 0.0f);
		gShapesUniformData[CYLINDER_TOP_BASE].model = Transpose(model);
		gShapesUniformData[CYLINDER_TOP_BASE].isShape2D = false;

		model = mat4::Scale(1.0f, 1.0f, 1.0f) * mat4::Translate(6.0f, 0.0f, 0.0f);
		gShapesUniformData[CYLINDER_BOTTOM_BASE].model = Transpose(model);
		gShapesUniformData[CYLINDER_BOTTOM_BASE].isShape2D = false;

		model = mat4::Scale(1.0f, 1.0f, 1.0f) * mat4::Translate(9.0f, 0.0f, 0.0f);
		gShapesUniformData[CYLINDER_BASE].model = Transpose(model);
		gShapesUniformData[CYLINDER_BASE].isShape2D = false;

		model = mat4::Scale(1.0f, 1.0f, 1.0f) * mat4::Translate(0.0f, -2.0f, 0.0f);
		gShapesUniformData[CONE].model = Transpose(model);
		gShapesUniformData[CONE].isShape2D = false;

		model = mat4::Scale(1.0f, 1.0f, 1.0f) * mat4::Translate(3.0f, -2.0f, 0.0f);
		gShapesUniformData[CONE_BASE].model = Transpose(model);
		gShapesUniformData[CONE_BASE].isShape2D = false;

		model = mat4::Scale(1.0f, 1.0f, 1.0f) * mat4::Translate(5.0f, -4.0f, 0.0f);
		gShapesUniformData[TORUS].model = Transpose(model);
		gShapesUniformData[TORUS].isShape2D = false;

		gPerFrameUniformData.view = Transpose(gCamera.viewMat);
		gPerFrameUniformData.projection = Transpose(gCamera.perspectiveProjMat);

		model = mat4::Scale(1000.0f, 1000.0f, 1000.0f);
		gSkyBoxUniformData.model = Transpose(model);
		gSkyBoxUniformData.isShape2D = true;

		mat4 viewMat = gCamera.viewMat;
		viewMat.SetRow(3, 0.0f, 0.0f, 0.0f, 1.0f);
		gSkyboxPerFrameUniformData.view = Transpose(viewMat);
		gSkyboxPerFrameUniformData.projection = Transpose(gCamera.perspectiveProjMat);
	}

	void Draw() override
	{
		CommandBuffer* pCommandBuffer = &gGraphicsCommandBuffers[gCurrentFrame];
		WaitForFence(&gRenderer, &pCommandBuffer->fence);

		void* data = nullptr;

		MapMemory(&gRenderer, &gPerFrameBuffer[gCurrentFrame], &data);
		memcpy(data, &gPerFrameUniformData, sizeof(PerFrameUniformData));
		UnmapMemory(&gRenderer, &gPerFrameBuffer[gCurrentFrame]);

		for (uint32_t i = 0; i < MAX_SHAPES; ++i)
		{
			MapMemory(&gRenderer, &gShapesUniformBuffers[gCurrentFrame * MAX_SHAPES + i], &data);
			memcpy(data, &gShapesUniformData[i], sizeof(PerObjectUniformData));
			UnmapMemory(&gRenderer, &gShapesUniformBuffers[gCurrentFrame * MAX_SHAPES + i]);
		}

		MapMemory(&gRenderer, &gSkyboxUniformBuffer[gCurrentFrame], &data);
		memcpy(data, &gSkyBoxUniformData, sizeof(PerObjectUniformData));
		UnmapMemory(&gRenderer, &gSkyboxUniformBuffer[gCurrentFrame]);

		MapMemory(&gRenderer, &gSkyboxPerFrameBuffer[gCurrentFrame], &data);
		memcpy(data, &gSkyboxPerFrameUniformData, sizeof(PerFrameUniformData));
		UnmapMemory(&gRenderer, &gSkyboxPerFrameBuffer[gCurrentFrame]);

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

		BindVertexBuffer(pCommandBuffer, sizeof(Vertex), 0, 0, &gVertexBuffer);
		BindIndexBuffer(pCommandBuffer, 0, INDEX_TYPE_UINT32, &gIndexBuffer);

		//Draw Shapes
		BindPipeline(pCommandBuffer, &gShapesPipeline);
		BindDescriptorSet(pCommandBuffer, 0, 0, &gDescriptorSetPerNone);

		for (uint32_t i = 0; i < MAX_SHAPES; ++i)
		{
			BindDescriptorSet(pCommandBuffer, gCurrentFrame * MAX_SHAPES + i, 1, &gDescriptorSetPerFrame);
			DrawIndexedInstanced(pCommandBuffer, gIndexCounts[i], 1, gIndexOffsets[i], gVertexOffsets[i], 0);
		}

		//Draw Skybox
		BindPipeline(pCommandBuffer, &gSkyboxPipeline);
		BindDescriptorSet(pCommandBuffer, 1, 0, &gDescriptorSetPerNone);
		BindDescriptorSet(pCommandBuffer, MAX_SHAPES * gNumFrames + gCurrentFrame, 1, &gDescriptorSetPerFrame);
		DrawIndexedInstanced(pCommandBuffer, gIndexCounts[BOX], 1, gIndexOffsets[BOX], gVertexOffsets[BOX], 0);

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
	Shapes shapes;
	shapes.appName = "Shapes";
	return WindowsMain(&shapes);
};
