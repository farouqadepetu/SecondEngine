#include "../../../SecondEngine/SEApp.h"
#include "../../../SecondEngine/Renderer/SERenderer.h"
#include "../../../SecondEngine/Math/SEMath_Header.h"
#include "../../../SecondEngine/Renderer/SECamera.h"
#include "../../../SecondEngine/Time/SETimer.h"
#include "../../../SecondEngine/Shapes/SEShapes.h"
#include "../../../SecondEngine/UI/SEUI.h"

Renderer gRenderer;

Queue gGraphicsQueue;

SwapChain gSwapChain;

RenderTarget gDepthBuffer;

RootSignature gGraphicsRootSignature;

Shader gShapesVertexShader;
Shader gShapes2DPixelShader;
Shader gShapes3DPixelShader;
Pipeline gShapes2DPipeline;
Pipeline gShapes3DPipeline;

Shader gShapesWirePixelShader;
Pipeline gShapesWirePipeline;

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

const char* gShapeNames[] =
{
	"EQUILATERAL_TRIANGLE",
	"RIGHT_TRIANGLE",
	"QUAD",
	"CIRCLE",
	"BOX",
	"SQUARE_PYRAMID",
	"TRIANGULAR_PYRAMID",
	"SPHERE",
	"HEMI_SPHERE",
	"HEMI_SPHERE_BASE",
	"CYLINDER",
	"CYLINDER_TOP_BASE",
	"CYLINDER_BOTTOM_BASE",
	"CYLINDER_BASE",
	"CONE",
	"CONE_BASE",
	"TORUS"
};
uint32_t gCurrentShape = 0;

struct PerObjectUniformData
{
	mat4 model[2];
};

PerObjectUniformData gShapesUniformData;
Buffer gShapesUniformBuffers[gNumFrames];

PerFrameUniformData gSkyboxPerFrameUniformData;
Buffer gSkyboxPerFrameBuffer[gNumFrames];

uint32_t gVertexCounts[MAX_SHAPES];
uint32_t gVertexOffsets[MAX_SHAPES];
uint32_t gIndexOffsets[MAX_SHAPES];
uint32_t gIndexCounts[MAX_SHAPES];

MainComponent gFillWindow;
MainComponent gShapeWindow;

int32_t gFillMode;

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

		ShaderInfo shaderInfo{};

		shaderInfo.filename = "shapes.vert";
		shaderInfo.type = SHADER_TYPE_VERTEX;
		CreateShader(&gRenderer, &shaderInfo, &gShapesVertexShader);

		shaderInfo.filename = "shapes2D.frag";
		shaderInfo.type = SHADER_TYPE_PIXEL;
		CreateShader(&gRenderer, &shaderInfo, &gShapes2DPixelShader);

		shaderInfo.filename = "shapes3D.frag";
		shaderInfo.type = SHADER_TYPE_PIXEL;
		CreateShader(&gRenderer, &shaderInfo, &gShapes3DPixelShader);

		shaderInfo.filename = "shapes_wire.frag";
		shaderInfo.type = SHADER_TYPE_PIXEL;
		CreateShader(&gRenderer, &shaderInfo, &gShapesWirePixelShader);

		shaderInfo.filename = "skybox.vert";
		shaderInfo.type = SHADER_TYPE_VERTEX;
		CreateShader(&gRenderer, &shaderInfo, &gSkyboxVertexShader);

		shaderInfo.filename = "skybox.frag";
		shaderInfo.type = SHADER_TYPE_PIXEL;
		CreateShader(&gRenderer, &shaderInfo, &gSkyboxPixelShader);

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
		vertexInputInfo.vertexAttributes[2].semanticName = "TANGENT";
		vertexInputInfo.vertexAttributes[2].semanticIndex = 0;
		vertexInputInfo.vertexAttributes[2].format = TinyImageFormat_R32G32B32A32_SFLOAT;
		vertexInputInfo.vertexAttributes[2].offset = 32;

		vertexInputInfo.vertexAttributes[3].binding = 0;
		vertexInputInfo.vertexAttributes[3].location = 3;
		vertexInputInfo.vertexAttributes[3].semanticName = "TEXCOORD";
		vertexInputInfo.vertexAttributes[3].semanticIndex = 0;
		vertexInputInfo.vertexAttributes[3].format = TinyImageFormat_R32G32_SFLOAT;
		vertexInputInfo.vertexAttributes[3].offset = 48;

		vertexInputInfo.numVertexAttributes = 4;

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
		graphicsPipelineInfo.pPixelShader = &gShapes2DPixelShader;
		graphicsPipelineInfo.numRenderTargets = 1;
		graphicsPipelineInfo.renderTargetFormat[0] = gSwapChain.pRenderTargets[0].info.format;
		graphicsPipelineInfo.depthInfo.depthTestEnable = true;
		graphicsPipelineInfo.depthInfo.depthWriteEnable = true;
		graphicsPipelineInfo.depthInfo.depthFunction = DEPTH_FUNCTION_LESS;
		graphicsPipelineInfo.depthFormat = gDepthBuffer.info.format;
		graphicsPipelineInfo.pVertexInputInfo = &vertexInputInfo;
		graphicsPipelineInfo.pRootSignature = &gGraphicsRootSignature;
		CreatePipeline(&gRenderer, &graphicsPipelineInfo, &gShapes2DPipeline);

		graphicsPipelineInfo.pPixelShader = &gShapes3DPixelShader;
		CreatePipeline(&gRenderer, &graphicsPipelineInfo, &gShapes3DPipeline);

		graphicsPipelineInfo.rasInfo.fillMode = FILL_MODE_WIREFRAME;
		graphicsPipelineInfo.pPixelShader = &gShapesWirePixelShader;
		CreatePipeline(&gRenderer, &graphicsPipelineInfo, &gShapesWirePipeline);

		graphicsPipelineInfo.rasInfo.fillMode = FILL_MODE_SOLID;
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
		CreateEquilateralTriangle(&gVertices, &gIndices, &gVertexCounts[EQUILATERAL_TRIANGLE], &gIndexCounts[EQUILATERAL_TRIANGLE]);

		gVertexOffsets[RIGHT_TRIANGLE] = arrlenu(gVertices);
		gIndexOffsets[RIGHT_TRIANGLE] = arrlenu(gIndices);
		CreateRightTriangle(&gVertices, &gIndices, &gVertexCounts[RIGHT_TRIANGLE], &gIndexCounts[RIGHT_TRIANGLE]);

		gVertexOffsets[QUAD] = arrlenu(gVertices);
		gIndexOffsets[QUAD] = arrlenu(gIndices);
		CreateQuad(&gVertices, &gIndices, &gVertexCounts[QUAD], &gIndexCounts[QUAD]);

		gVertexOffsets[CIRCLE] = arrlenu(gVertices);
		gIndexOffsets[CIRCLE] = arrlenu(gIndices);
		CreateCircle(&gVertices, &gIndices, &gVertexCounts[CIRCLE], &gIndexCounts[CIRCLE]);

		gVertexOffsets[BOX] = arrlenu(gVertices);
		gIndexOffsets[BOX] = arrlenu(gIndices);
		CreateBox(&gVertices, &gIndices, &gVertexCounts[BOX], &gIndexCounts[BOX]);

		gVertexOffsets[SQUARE_PYRAMID] = arrlenu(gVertices);
		gIndexOffsets[SQUARE_PYRAMID] = arrlenu(gIndices);
		CreateSquarePyramid(&gVertices, &gIndices, &gVertexCounts[SQUARE_PYRAMID], &gIndexCounts[SQUARE_PYRAMID]);

		gVertexOffsets[TRIANGULAR_PYRAMID] = arrlenu(gVertices);
		gIndexOffsets[TRIANGULAR_PYRAMID] = arrlenu(gIndices);
		CreateTriangularPyramid(&gVertices, &gIndices, &gVertexCounts[TRIANGULAR_PYRAMID], &gIndexCounts[TRIANGULAR_PYRAMID]);

		gVertexOffsets[SPHERE] = arrlenu(gVertices);
		gIndexOffsets[SPHERE] = arrlenu(gIndices);
		CreateSphere(&gVertices, &gIndices, &gVertexCounts[SPHERE], &gIndexCounts[SPHERE]);

		gVertexOffsets[HEMI_SPHERE] = arrlenu(gVertices);
		gIndexOffsets[HEMI_SPHERE] = arrlenu(gIndices);
		CreateHemiSphere(&gVertices, &gIndices, &gVertexCounts[HEMI_SPHERE], &gIndexCounts[HEMI_SPHERE], false);

		gVertexOffsets[HEMI_SPHERE_BASE] = arrlenu(gVertices);
		gIndexOffsets[HEMI_SPHERE_BASE] = arrlenu(gIndices);
		CreateHemiSphere(&gVertices, &gIndices, &gVertexCounts[HEMI_SPHERE_BASE], &gIndexCounts[HEMI_SPHERE_BASE], true);

		gVertexOffsets[CYLINDER] = arrlenu(gVertices);
		gIndexOffsets[CYLINDER] = arrlenu(gIndices);
		CreateCylinder(&gVertices, &gIndices, &gVertexCounts[CYLINDER], &gIndexCounts[CYLINDER], false, false);

		gVertexOffsets[CYLINDER_TOP_BASE] = arrlenu(gVertices);
		gIndexOffsets[CYLINDER_TOP_BASE] = arrlenu(gIndices);
		CreateCylinder(&gVertices, &gIndices, &gVertexCounts[CYLINDER_TOP_BASE], &gIndexCounts[CYLINDER_TOP_BASE], true, false);

		gVertexOffsets[CYLINDER_BOTTOM_BASE] = arrlenu(gVertices);
		gIndexOffsets[CYLINDER_BOTTOM_BASE] = arrlenu(gIndices);
		CreateCylinder(&gVertices, &gIndices, &gVertexCounts[CYLINDER_BOTTOM_BASE], &gIndexCounts[CYLINDER_BOTTOM_BASE], false, true);

		gVertexOffsets[CYLINDER_BASE] = arrlenu(gVertices);
		gIndexOffsets[CYLINDER_BASE] = arrlenu(gIndices);
		CreateCylinder(&gVertices, &gIndices, &gVertexCounts[CYLINDER_BASE], &gIndexCounts[CYLINDER_BASE], true, true);

		gVertexOffsets[CONE] = arrlenu(gVertices);
		gIndexOffsets[CONE] = arrlenu(gIndices);
		CreateCone(&gVertices, &gIndices, &gVertexCounts[CONE], &gIndexCounts[CONE], false);

		gVertexOffsets[CONE_BASE] = arrlenu(gVertices);
		gIndexOffsets[CONE_BASE] = arrlenu(gIndices);
		CreateCone(&gVertices, &gIndices, &gVertexCounts[CONE_BASE], &gIndexCounts[CONE_BASE], true);

		gVertexOffsets[TORUS] = arrlenu(gVertices);
		gIndexOffsets[TORUS] = arrlenu(gIndices);
		CreateTorus(&gVertices, &gIndices, &gVertexCounts[TORUS], &gIndexCounts[TORUS], 2.0f, 1.0f);

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
		uniformSetInfo.numSets = 4;
		uniformSetInfo.updateFrequency = UPDATE_FREQUENCY_PER_FRAME;
		CreateDescriptorSet(&gRenderer, &uniformSetInfo, &gDescriptorSetPerFrame);

		DescriptorSetInfo texturesSetInfo{};
		texturesSetInfo.pRootSignature = &gGraphicsRootSignature;
		texturesSetInfo.numSets = 2;
		texturesSetInfo.updateFrequency = UPDATE_FREQUENCY_PER_NONE;
		CreateDescriptorSet(&gRenderer, &texturesSetInfo, &gDescriptorSetPerNone);

		for (uint32_t i = 0; i < gNumFrames; ++i)
		{
			UpdateDescriptorSetInfo updatePerFrame[2]{};
			updatePerFrame[0].binding = 0;
			updatePerFrame[0].type = UPDATE_TYPE_UNIFORM_BUFFER;
			updatePerFrame[0].pBuffer = &gPerFrameBuffer[i];

			updatePerFrame[1].binding = 1;
			updatePerFrame[1].type = UPDATE_TYPE_UNIFORM_BUFFER;
			updatePerFrame[1].pBuffer = &gShapesUniformBuffers[i];

			UpdateDescriptorSet(&gRenderer, &gDescriptorSetPerFrame, i, 2, updatePerFrame);

			updatePerFrame[0].binding = 0;
			updatePerFrame[0].type = UPDATE_TYPE_UNIFORM_BUFFER;
			updatePerFrame[0].pBuffer = &gSkyboxPerFrameBuffer[i];
			UpdateDescriptorSet(&gRenderer, &gDescriptorSetPerFrame, i + 2, 2, updatePerFrame);
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

		LookAt(&gCamera, vec3(0.0f, 0.0f, -6.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));

		gCamera.vFov = 45.0f;
		gCamera.nearP = 1.0f;
		gCamera.farP = 100.0f;

		UIDesc uiDesc{};
		uiDesc.pWindow = pWindow;
		uiDesc.pQueue = &gGraphicsQueue;
		uiDesc.numFrames = gNumFrames;
		uiDesc.renderTargetFormat = gSwapChain.info.format;
		uiDesc.depthFormat = gDepthBuffer.info.format;
		uiDesc.numImages = gSwapChain.numRenderTargets;
		InitUI(&gRenderer, &uiDesc);

		MainComponentInfo mcInfo;
		mcInfo.pLabel = "FILL MODE";
		mcInfo.position = vec2(GetWidth(pWindow) - 125.0f, 100.0f);
		mcInfo.size = vec2(125.0f, 90.0f);
		mcInfo.flags = MAIN_COMPONENT_FLAGS_NO_RESIZE | MAIN_COMPONENT_FLAGS_NO_SAVED_SETTINGS;
		CreateMainComponent(&mcInfo, &gFillWindow);

		mcInfo.pLabel = "##";
		mcInfo.position = vec2(GetWidth(pWindow) - 300.0f, 200.0f);
		mcInfo.size = vec2(300.0f, 90.0f);
		mcInfo.flags = MAIN_COMPONENT_FLAGS_NO_RESIZE | MAIN_COMPONENT_FLAGS_NO_SAVED_SETTINGS;
		CreateMainComponent(&mcInfo, &gShapeWindow);

		SubComponent radio0{};
		radio0.type = SUB_COMPONENT_TYPE_RADIO_BUTTON;
		radio0.radio.pLabel = "SOLID";
		radio0.radio.id = 0;
		radio0.radio.pData = &gFillMode;
		AddSubComponent(&gFillWindow, &radio0);

		SubComponent radio1{};
		radio1.type = SUB_COMPONENT_TYPE_RADIO_BUTTON;
		radio1.radio.pLabel = "WIREFRAME";
		radio1.radio.id = 1;
		radio1.radio.pData = &gFillMode;
		AddSubComponent(&gFillWindow, &radio1);

		SubComponent shapes{};
		shapes.type = SUB_COMPONENT_TYPE_DROPDOWN;
		shapes.dropDown.pLabel = "SHAPES";
		shapes.dropDown.numNames = MAX_SHAPES;
		shapes.dropDown.pData = &gCurrentShape;
		shapes.dropDown.pNames = gShapeNames;
		AddSubComponent(&gShapeWindow, &shapes);
	}

	void Exit() override
	{
		WaitQueueIdle(&gRenderer, &gGraphicsQueue);

		DestroyMainComponent(&gFillWindow);
		DestroyMainComponent(&gShapeWindow);
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
			DestroyBuffer(&gRenderer, &gShapesUniformBuffers[i]);
			DestroyBuffer(&gRenderer, &gSkyboxPerFrameBuffer[i]);
			DestroySemaphore(&gRenderer, &gImageAvailableSemaphores[i]);
			DestroyCommandBuffer(&gRenderer, &gGraphicsCommandBuffers[i]);
		}

		DestroyPipeline(&gRenderer, &gSkyboxPipeline);
		DestroyPipeline(&gRenderer, &gShapesWirePipeline);
		DestroyPipeline(&gRenderer, &gShapes3DPipeline);
		DestroyPipeline(&gRenderer, &gShapes2DPipeline);

		DestroyRootSignature(&gRenderer, &gGraphicsRootSignature);

		DestroyBuffer(&gRenderer, &gIndexBuffer);
		DestroyBuffer(&gRenderer, &gVertexBuffer);

		DestroyShader(&gRenderer, &gSkyboxVertexShader);
		DestroyShader(&gRenderer, &gSkyboxPixelShader);
		DestroyShader(&gRenderer, &gShapesWirePixelShader);
		DestroyShader(&gRenderer, &gShapes3DPixelShader);
		DestroyShader(&gRenderer, &gShapes2DPixelShader);
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
			LookAt(&gCamera, vec3(0.0f, 0.0f, -6.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
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

		gPerFrameUniformData.view = Transpose(gCamera.viewMat);
		gPerFrameUniformData.projection = Transpose(gCamera.perspectiveProjMat);

		mat4 model = mat4::Scale(1.0f, 1.0f, 1.0f) * mat4::Translate(0.0f, 0.0f, 0.0f);
		gShapesUniformData.model[0] = Transpose(model);

		model = mat4::Scale(1000.0f, 1000.0f, 1000.0f);
		gShapesUniformData.model[1] = Transpose(model);

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

		MapMemory(&gRenderer, &gShapesUniformBuffers[gCurrentFrame], &data);
		memcpy(data, &gShapesUniformData, sizeof(PerObjectUniformData));
		UnmapMemory(&gRenderer, &gShapesUniformBuffers[gCurrentFrame]);

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
		if (gFillMode == FILL_MODE_SOLID)
		{
			if (gCurrentShape < 4)
			{
				BindPipeline(pCommandBuffer, &gShapes2DPipeline);
			}
			else
			{
				BindPipeline(pCommandBuffer, &gShapes3DPipeline);
			}
		}
		else //gFillMode == FILL_MODE_WIREFRAME
		{
			BindPipeline(pCommandBuffer, &gShapesWirePipeline);
		}

		BindDescriptorSet(pCommandBuffer, 0, 0, &gDescriptorSetPerNone);
		BindDescriptorSet(pCommandBuffer, gCurrentFrame, 1, &gDescriptorSetPerFrame);
		DrawIndexedInstanced(pCommandBuffer, gIndexCounts[gCurrentShape], 1, gIndexOffsets[gCurrentShape], gVertexOffsets[gCurrentShape], 0);

		//Draw Skybox
		BindPipeline(pCommandBuffer, &gSkyboxPipeline);
		BindDescriptorSet(pCommandBuffer, 1, 0, &gDescriptorSetPerNone);
		BindDescriptorSet(pCommandBuffer, gCurrentFrame + 2, 1, &gDescriptorSetPerFrame);
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
