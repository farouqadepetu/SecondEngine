#include "../../../SecondEngine/SEApp.h"
#include "../../../SecondEngine/Renderer/SERenderer.h"
#include "../../../SecondEngine/Math/SEMath_Header.h"
#include "../../../SecondEngine/Renderer/SECamera.h"
#include "../../../SecondEngine/Time/SETimer.h"
#include "../../../SecondEngine/Shapes/SEShapes.h"
#include "../../../SecondEngine/UI/SEUI.h"
#include "../../../SecondEngine/Renderer/ShaderLibrary/lightSource.h"

Renderer gRenderer;

Queue gGraphicsQueue;

SwapChain gSwapChain;

RenderTarget gDepthBuffer;

RenderTarget gShadowMap;
RenderTarget gShadowMapPL[6];

RootSignature gGraphicsRootSignature;

Shader gObjectVS;
Shader gLightSourceVS;
Shader gShadowMapVS;
Shader gShadowDebugVS;

Shader gObjectPS;
Shader gLightSourcePS;
Shader gShadowMapPS;
Shader gShadowDebugPS;

Pipeline gObjectPipeline;
Pipeline gLightSourcePipeline;
Pipeline gShadowMapPipeline;
Pipeline gShadowDebugPipeline;

const uint32_t gNumFrames = 2;
const uint32_t gShadowWidth = 4096;
const uint32_t gShadowHeight = 4096;

Semaphore gImageAvailableSemaphores[gNumFrames];
CommandBuffer gGraphicsCommandBuffers[gNumFrames];

Buffer gVertexBuffer;
Buffer gIndexBuffer;

Sampler gSampler;
Sampler gSamplerComparison;

uint32_t gCurrentFrame = 0;

DescriptorSet gDescriptorSetPerFrame;
DescriptorSet gDescriptorSetPerNone;

Camera gCamera;
Camera gDLCamera;
Camera gPLCamera[6];

Vertex* gVertices = nullptr;
uint32_t* gIndices = nullptr;

struct CameraData
{
	mat4 cameraView;
	mat4 cameraProjection;
	vec4 cameraPos;
};

#define NUM_LIGHT_DATA 8
struct LightSourceData
{
	mat4 lightSourceModel[NUM_LIGHT_DATA];
	mat4 lightSourceView[NUM_LIGHT_DATA];
	mat4 lightSourceProjection[NUM_LIGHT_DATA];
	vec4 lightPosition[NUM_LIGHT_DATA];
};

struct PBRMaterial
{
	vec4 albedo;
	float metallic;
	float roughness;
	float ao;
};

enum
{
	WALL,
	SPHERE,
	CYLINDER,
	MAX_OBJECTS
};

enum
{
	DIRECTIONAL_LIGHT,
	POINT_LIGHT,
	SPOTLIGHT,
	MAX_LIGHT_SOURCES
};

#define NUM_OBJECTS 7
struct ObjectData
{
	mat4 objectModel[NUM_OBJECTS];
	mat4 objectInverseModel[NUM_OBJECTS];
	PBRMaterial material[NUM_OBJECTS];
};

PointLight gPointLight;
Buffer gPointLightUniformBuffer[gNumFrames];

DirectionalLight gDirectionalLight;
Buffer gDirectionalLightUniformBuffer[gNumFrames];

Spotlight gSpotlight;
Buffer gSpotlightUniformBuffer[gNumFrames];

LightSourceData gLightSourceData;
Buffer gLightSourceUniformBuffer[gNumFrames];

CameraData gCameraData;
Buffer gCameraUniformBuffer[gNumFrames];

ObjectData gObjectData;
Buffer gObjectUniformBuffers[gNumFrames];

uint32_t gVertexCounts[MAX_OBJECTS];
uint32_t gVertexOffsets[MAX_OBJECTS];
uint32_t gIndexOffsets[MAX_OBJECTS];
uint32_t gIndexCounts[MAX_OBJECTS];

MainComponent gGuiWindow;

const char* gLightSourceNames[] =
{
	"DIRECTIONAL",
	"POINT",
	"SPOTLIGHT",
};
uint32_t gCurrentLightSource = 0;

bool gShowLightSources = false;
bool gRotate = false;

float gInnerCutoffAngle = 2.0f;
float gOuterCutoffAngle = 5.0f;

bool gShowShadowDebug = false;

struct RootConstants
{
	uint32_t currentLightSource;
	float shadowBias;
	uint32_t objectIndex;
	uint32_t lightIndex;
	uint32_t debugIndex;
};

RootConstants gConstants;

float gAngle = 0.0f;

vec3 gLightColor = vec3(1.0f, 1.0f, 1.0f);
float gShadowBias = 0.01f;
int32_t gDebugIndex;

SubComponent gInnerCutoffSc{};
SubComponent gOuterCutoffSc{};

class Shadows : public App
{
public:
	void Init() override
	{
		InitSE();

		InitRenderer(&gRenderer, "Shadows");

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

		dbInfo.format = TinyImageFormat_D32_SFLOAT;
		dbInfo.width = gShadowWidth;
		dbInfo.height = gShadowHeight;
		dbInfo.clearValue.depth = 1.0f;
		dbInfo.clearValue.stencil = 0;
		dbInfo.initialState = RESOURCE_STATE_ALL_SHADER_RESOURCE;
		dbInfo.type = TEXTURE_TYPE_TEXTURE;
		CreateRenderTarget(&gRenderer, &dbInfo, &gShadowMap);

		for (uint32_t i = 0; i < 6; ++i)
		{
			CreateRenderTarget(&gRenderer, &dbInfo, &gShadowMapPL[i]);
		}

		ShaderInfo shaderInfo{};

		shaderInfo.filename = "object.vert";
		shaderInfo.type = SHADER_TYPE_VERTEX;
		CreateShader(&gRenderer, &shaderInfo, &gObjectVS);

		shaderInfo.filename = "lightSource.vert";
		shaderInfo.type = SHADER_TYPE_VERTEX;
		CreateShader(&gRenderer, &shaderInfo, &gLightSourceVS);

		shaderInfo.filename = "shadowMap.vert";
		shaderInfo.type = SHADER_TYPE_VERTEX;
		CreateShader(&gRenderer, &shaderInfo, &gShadowMapVS);

		shaderInfo.filename = "shadowDebug.vert";
		shaderInfo.type = SHADER_TYPE_VERTEX;
		CreateShader(&gRenderer, &shaderInfo, &gShadowDebugVS);

		shaderInfo.filename = "object.frag";
		shaderInfo.type = SHADER_TYPE_PIXEL;
		CreateShader(&gRenderer, &shaderInfo, &gObjectPS);

		shaderInfo.filename = "lightSource.frag";
		shaderInfo.type = SHADER_TYPE_PIXEL;
		CreateShader(&gRenderer, &shaderInfo, &gLightSourcePS);

		shaderInfo.filename = "shadowMap.frag";
		shaderInfo.type = SHADER_TYPE_PIXEL;
		CreateShader(&gRenderer, &shaderInfo, &gShadowMapPS);

		shaderInfo.filename = "shadowDebug.frag";
		shaderInfo.type = SHADER_TYPE_PIXEL;
		CreateShader(&gRenderer, &shaderInfo, &gShadowDebugPS);

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

		RootParameterInfo rootParameterInfos[10]{};

		//Camera
		rootParameterInfos[0].binding = 0;
		rootParameterInfos[0].baseRegister = 0;
		rootParameterInfos[0].registerSpace = 0;
		rootParameterInfos[0].numDescriptors = 1;
		rootParameterInfos[0].stages = STAGE_VERTEX | STAGE_PIXEL;
		rootParameterInfos[0].type = DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		rootParameterInfos[0].updateFrequency = UPDATE_FREQUENCY_PER_FRAME;

		//Object
		rootParameterInfos[1].binding = 1;
		rootParameterInfos[1].baseRegister = 1;
		rootParameterInfos[1].registerSpace = 0;
		rootParameterInfos[1].numDescriptors = 1;
		rootParameterInfos[1].stages = STAGE_VERTEX | STAGE_PIXEL;
		rootParameterInfos[1].type = DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		rootParameterInfos[1].updateFrequency = UPDATE_FREQUENCY_PER_FRAME;

		//Light Source
		rootParameterInfos[2].binding = 2;
		rootParameterInfos[2].baseRegister = 2;
		rootParameterInfos[2].registerSpace = 0;
		rootParameterInfos[2].numDescriptors = 1;
		rootParameterInfos[2].stages = STAGE_VERTEX | STAGE_PIXEL;
		rootParameterInfos[2].type = DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		rootParameterInfos[2].updateFrequency = UPDATE_FREQUENCY_PER_FRAME;

		//Point light
		rootParameterInfos[3].binding = 3;
		rootParameterInfos[3].baseRegister = 3;
		rootParameterInfos[3].registerSpace = 0;
		rootParameterInfos[3].numDescriptors = 1;
		rootParameterInfos[3].stages = STAGE_VERTEX | STAGE_PIXEL;
		rootParameterInfos[3].type = DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		rootParameterInfos[3].updateFrequency = UPDATE_FREQUENCY_PER_FRAME;

		//Directional light
		rootParameterInfos[4].binding = 4;
		rootParameterInfos[4].baseRegister = 4;
		rootParameterInfos[4].registerSpace = 0;
		rootParameterInfos[4].numDescriptors = 1;
		rootParameterInfos[4].stages = STAGE_VERTEX | STAGE_PIXEL;
		rootParameterInfos[4].type = DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		rootParameterInfos[4].updateFrequency = UPDATE_FREQUENCY_PER_FRAME;

		//Spotlight
		rootParameterInfos[5].binding = 5;
		rootParameterInfos[5].baseRegister = 5;
		rootParameterInfos[5].registerSpace = 0;
		rootParameterInfos[5].numDescriptors = 1;
		rootParameterInfos[5].stages = STAGE_VERTEX | STAGE_PIXEL;
		rootParameterInfos[5].type = DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		rootParameterInfos[5].updateFrequency = UPDATE_FREQUENCY_PER_FRAME;

		//Shadow map texture
		rootParameterInfos[6].binding = 0;
		rootParameterInfos[6].baseRegister = 0;
		rootParameterInfos[6].registerSpace = 0;
		rootParameterInfos[6].numDescriptors = 1;
		rootParameterInfos[6].stages = STAGE_PIXEL;
		rootParameterInfos[6].type = DESCRIPTOR_TYPE_TEXTURE;
		rootParameterInfos[6].updateFrequency = UPDATE_FREQUENCY_PER_NONE;

		//Shadow PL map texture
		rootParameterInfos[7].binding = 1;
		rootParameterInfos[7].baseRegister = 1;
		rootParameterInfos[7].registerSpace = 0;
		rootParameterInfos[7].numDescriptors = 6;
		rootParameterInfos[7].stages = STAGE_PIXEL;
		rootParameterInfos[7].type = DESCRIPTOR_TYPE_TEXTURE;
		rootParameterInfos[7].updateFrequency = UPDATE_FREQUENCY_PER_NONE;

		//Sampler
		rootParameterInfos[8].binding = 5;
		rootParameterInfos[8].baseRegister = 0;
		rootParameterInfos[8].registerSpace = 0;
		rootParameterInfos[8].numDescriptors = 1;
		rootParameterInfos[8].stages = STAGE_PIXEL;
		rootParameterInfos[8].type = DESCRIPTOR_TYPE_SAMPLER;
		rootParameterInfos[8].updateFrequency = UPDATE_FREQUENCY_PER_NONE;

		//Sampler Comparison
		rootParameterInfos[9].binding = 6;
		rootParameterInfos[9].baseRegister = 1;
		rootParameterInfos[9].registerSpace = 0;
		rootParameterInfos[9].numDescriptors = 1;
		rootParameterInfos[9].stages = STAGE_PIXEL;
		rootParameterInfos[9].type = DESCRIPTOR_TYPE_SAMPLER;
		rootParameterInfos[9].updateFrequency = UPDATE_FREQUENCY_PER_NONE;

		RootConstantsInfo rootConstantInfo{};
		rootConstantInfo.numValues = 5;
		rootConstantInfo.baseRegister = 6;
		rootConstantInfo.registerSpace = 0;
		rootConstantInfo.stride = sizeof(RootConstants);
		rootConstantInfo.stages = STAGE_VERTEX | STAGE_PIXEL;

		RootSignatureInfo graphicsRootSignatureInfo{};
		graphicsRootSignatureInfo.pRootParameterInfos = rootParameterInfos;
		graphicsRootSignatureInfo.numRootParameterInfos = 10;
		graphicsRootSignatureInfo.useRootConstants = true;
		graphicsRootSignatureInfo.rootConstantsInfo = rootConstantInfo;
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
		graphicsPipelineInfo.pVertexShader = &gObjectVS;
		graphicsPipelineInfo.pPixelShader = &gObjectPS;
		graphicsPipelineInfo.numRenderTargets = 1;
		graphicsPipelineInfo.renderTargetFormat[0] = gSwapChain.pRenderTargets[0].info.format;
		graphicsPipelineInfo.depthInfo.depthTestEnable = true;
		graphicsPipelineInfo.depthInfo.depthWriteEnable = true;
		graphicsPipelineInfo.depthInfo.depthFunction = DEPTH_FUNCTION_LESS;
		graphicsPipelineInfo.depthFormat = gDepthBuffer.info.format;
		graphicsPipelineInfo.pVertexInputInfo = &vertexInputInfo;
		graphicsPipelineInfo.pRootSignature = &gGraphicsRootSignature;
		CreatePipeline(&gRenderer, &graphicsPipelineInfo, &gObjectPipeline);

		graphicsPipelineInfo.pVertexShader = &gLightSourceVS;
		graphicsPipelineInfo.pPixelShader = &gLightSourcePS;
		CreatePipeline(&gRenderer, &graphicsPipelineInfo, &gLightSourcePipeline);

		graphicsPipelineInfo.pVertexShader = &gShadowMapVS;
		graphicsPipelineInfo.pPixelShader = &gShadowMapPS;
		graphicsPipelineInfo.numRenderTargets = 0;
		CreatePipeline(&gRenderer, &graphicsPipelineInfo, &gShadowMapPipeline);

		graphicsPipelineInfo.pVertexShader = &gShadowDebugVS;
		graphicsPipelineInfo.pPixelShader = &gShadowDebugPS;
		graphicsPipelineInfo.numRenderTargets = 1;
		CreatePipeline(&gRenderer, &graphicsPipelineInfo, &gShadowDebugPipeline);

		for (uint32_t i = 0; i < gNumFrames; ++i)
		{
			CreateCommandBuffer(&gRenderer, QUEUE_TYPE_GRAPHICS, &gGraphicsCommandBuffers[i]);
			CreateSemaphore(&gRenderer, &gImageAvailableSemaphores[i]);
		}

		gVertexOffsets[WALL] = arrlenu(gVertices);
		gIndexOffsets[WALL] = arrlenu(gIndices);
		CreateQuad(&gVertices, &gIndices, &gVertexCounts[WALL], & gIndexCounts[WALL]);

		gVertexOffsets[SPHERE] = arrlenu(gVertices);
		gIndexOffsets[SPHERE] = arrlenu(gIndices);
		CreateSphere(&gVertices, &gIndices, &gVertexCounts[SPHERE], &gIndexCounts[SPHERE]);

		gVertexOffsets[CYLINDER] = arrlenu(gVertices);
		gIndexOffsets[CYLINDER] = arrlenu(gIndices);
		CreateCylinder(&gVertices, &gIndices, &gVertexCounts[CYLINDER], &gIndexCounts[CYLINDER], true, true);

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

		BufferInfo ubInfo{};
		ubInfo.type = BUFFER_TYPE_UNIFORM;
		ubInfo.usage = MEMORY_USAGE_CPU_TO_GPU;
		ubInfo.data = nullptr;

		for (uint32_t i = 0; i < gNumFrames; ++i)
		{
			ubInfo.size = sizeof(ObjectData);
			CreateBuffer(&gRenderer, &ubInfo, &gObjectUniformBuffers[i]);

			ubInfo.size = sizeof(CameraData);
			CreateBuffer(&gRenderer, &ubInfo, &gCameraUniformBuffer[i]);

			ubInfo.size = sizeof(PointLight);
			CreateBuffer(&gRenderer, &ubInfo, &gPointLightUniformBuffer[i]);

			ubInfo.size = sizeof(DirectionalLight);
			CreateBuffer(&gRenderer, &ubInfo, &gDirectionalLightUniformBuffer[i]);

			ubInfo.size = sizeof(Spotlight);
			CreateBuffer(&gRenderer, &ubInfo, &gSpotlightUniformBuffer[i]);

			ubInfo.size = sizeof(LightSourceData);
			CreateBuffer(&gRenderer, &ubInfo, &gLightSourceUniformBuffer[i]);
		}

		//Right Wall
		gObjectData.material[0].albedo = vec4(0.0f, 0.0f, 0.5f, 1.0f);
		gObjectData.material[0].roughness = 0.5f;
		gObjectData.material[0].ao = 0.05f;
		gObjectData.material[0].metallic = 0.0f;

		//Left Wall
		gObjectData.material[1].albedo = vec4(0.5f, 0.0f, 0.0f, 1.0f);
		gObjectData.material[1].roughness = 0.5f;
		gObjectData.material[1].ao = 0.05f;
		gObjectData.material[1].metallic = 0.0f;

		//Top Wall
		gObjectData.material[2].albedo = vec4(0.0f, 0.0f, 0.0f, 1.0f);
		gObjectData.material[2].roughness = 0.5f;
		gObjectData.material[2].ao = 0.05f;
		gObjectData.material[2].metallic = 0.0f;

		//Bottom Wall
		gObjectData.material[3].albedo = vec4(0.2f, 0.2f, 0.2f, 1.0f);
		gObjectData.material[3].roughness = 0.5f;
		gObjectData.material[3].ao = 0.05f;
		gObjectData.material[3].metallic = 0.0f;

		//Back Wall
		gObjectData.material[4].albedo = vec4(0.2f, 0.2f, 0.2f, 1.0f);
		gObjectData.material[4].roughness = 0.5f;
		gObjectData.material[4].ao = 0.05f;
		gObjectData.material[4].metallic = 0.0f;

		//Sphere
		gObjectData.material[5].albedo = vec4(0.5f, 0.5f, 0.0f, 1.0f);
		gObjectData.material[5].roughness = 0.5f;
		gObjectData.material[5].ao = 0.05f;
		gObjectData.material[5].metallic = 0.0f;

		//Cylinder
		gObjectData.material[6].albedo = vec4(0.0f, 0.5f, 0.5f, 1.0f);
		gObjectData.material[6].roughness = 0.5f;
		gObjectData.material[6].ao = 0.05f;
		gObjectData.material[6].metallic = 0.0f;

		SamplerInfo samplerInfo{};
		samplerInfo.magFilter = FILTER_LINEAR;
		samplerInfo.minFilter = FILTER_LINEAR;
		samplerInfo.u = ADDRESS_MODE_CLAMP_TO_BORDER;
		samplerInfo.v = ADDRESS_MODE_CLAMP_TO_BORDER;
		samplerInfo.w = ADDRESS_MODE_CLAMP_TO_BORDER;
		samplerInfo.mipMapMode = MIPMAP_MODE_LINEAR;
		samplerInfo.comparisonFunction = DEPTH_FUNCTION_NONE;
		samplerInfo.maxAnisotropy = 0.0f;
		samplerInfo.mipLoadBias = 0.0f;
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = 0.0f;
		samplerInfo.borderColor[0] = 1.0f;
		samplerInfo.borderColor[1] = 1.0f;
		samplerInfo.borderColor[2] = 1.0f;
		samplerInfo.borderColor[3] = 1.0f;
		CreateSampler(&gRenderer, &samplerInfo, &gSampler);

		samplerInfo.comparisonFunction = DEPTH_FUNCTION_LESS_OR_EQUAL;
		CreateSampler(&gRenderer, &samplerInfo, &gSamplerComparison);

		DescriptorSetInfo setInfo{};
		setInfo.pRootSignature = &gGraphicsRootSignature;
		setInfo.numSets = 2;
		setInfo.updateFrequency = UPDATE_FREQUENCY_PER_FRAME;
		CreateDescriptorSet(&gRenderer, &setInfo, &gDescriptorSetPerFrame);

		setInfo.pRootSignature = &gGraphicsRootSignature;
		setInfo.numSets = 1;
		setInfo.updateFrequency = UPDATE_FREQUENCY_PER_NONE;
		CreateDescriptorSet(&gRenderer, &setInfo, &gDescriptorSetPerNone);

		//Objects with Directional Shadow Map
		for (uint32_t i = 0; i < gNumFrames; ++i)
		{
			UpdateDescriptorSetInfo updatePerFrame[6]{};
			updatePerFrame[0].binding = 0;
			updatePerFrame[0].type = UPDATE_TYPE_UNIFORM_BUFFER;
			updatePerFrame[0].pBuffer = &gCameraUniformBuffer[i];

			updatePerFrame[1].binding = 1;
			updatePerFrame[1].type = UPDATE_TYPE_UNIFORM_BUFFER;
			updatePerFrame[1].pBuffer = &gObjectUniformBuffers[i];

			updatePerFrame[2].binding = 2;
			updatePerFrame[2].type = UPDATE_TYPE_UNIFORM_BUFFER;
			updatePerFrame[2].pBuffer = &gLightSourceUniformBuffer[i];

			updatePerFrame[3].binding = 3;
			updatePerFrame[3].type = UPDATE_TYPE_UNIFORM_BUFFER;
			updatePerFrame[3].pBuffer = &gPointLightUniformBuffer[i];

			updatePerFrame[4].binding = 4;
			updatePerFrame[4].type = UPDATE_TYPE_UNIFORM_BUFFER;
			updatePerFrame[4].pBuffer = &gDirectionalLightUniformBuffer[i];

			updatePerFrame[5].binding = 5;
			updatePerFrame[5].type = UPDATE_TYPE_UNIFORM_BUFFER;
			updatePerFrame[5].pBuffer = &gSpotlightUniformBuffer[i];

			UpdateDescriptorSet(&gRenderer, &gDescriptorSetPerFrame, i, 6, updatePerFrame);
		}

		UpdateDescriptorSetInfo updatePerNone[9]{};
		updatePerNone[0].binding = 0;
		updatePerNone[0].type = UPDATE_TYPE_TEXTURE;
		updatePerNone[0].pTexture = &gShadowMap.texture;

		for (uint32_t i = 0; i < 6; ++i)
		{
			updatePerNone[i + 1].binding = i + 1;
			updatePerNone[i + 1].type = UPDATE_TYPE_TEXTURE;
			updatePerNone[i + 1].pTexture = &gShadowMapPL[i].texture;
		}

		updatePerNone[7].binding = 8;
		updatePerNone[7].type = UPDATE_TYPE_SAMPLER;
		updatePerNone[7].pSampler = &gSampler;

		updatePerNone[8].binding = 9;
		updatePerNone[8].type = UPDATE_TYPE_SAMPLER;
		updatePerNone[8].pSampler = &gSamplerComparison;
		UpdateDescriptorSet(&gRenderer, &gDescriptorSetPerNone, 0, 9, updatePerNone);
	
		LookAt(&gCamera, vec3(0.0f, -3.0f, -6.0f), vec3(0.0f, -3.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f));
		gCamera.vFov = 45.0f;
		gCamera.nearP = 1.0f;
		gCamera.farP = 100.0f;

		LookAt(&gDLCamera, vec3(0.0f, 4.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f));
		gDLCamera.width = 15.0f;
		gDLCamera.height = 15.0f;
		gDLCamera.nearP = 1.0f;
		gDLCamera.farP = 10.0f;

		for (uint32_t i = 0; i < 6; ++i)
		{
			gPLCamera[i].vFov = 90.0f;
			gPLCamera[i].nearP = 1.0f;
			gPLCamera[i].farP = 10.0f;
		}

		UIDesc uiInfo{};
		uiInfo.pWindow = pWindow;
		uiInfo.pQueue = &gGraphicsQueue;
		uiInfo.numFrames = gNumFrames;
		uiInfo.renderTargetFormat = gSwapChain.info.format;
		uiInfo.depthFormat = gDepthBuffer.info.format;
		uiInfo.numImages = gSwapChain.numRenderTargets;
		InitUI(&gRenderer, &uiInfo);

		MainComponentInfo mcInfo{};
		mcInfo.pLabel = "##";
		mcInfo.position = vec2(GetWidth(pWindow) - 200.0f, 175.0f);
		mcInfo.size = vec2(200.0f, 500.0f);
		mcInfo.flags = MAIN_COMPONENT_FLAGS_NO_RESIZE;
		CreateMainComponent(&mcInfo, &gGuiWindow);

		SubComponent lightSource{};
		lightSource.type = SUB_COMPONENT_TYPE_DROPDOWN;
		lightSource.dropDown.pLabel = "Light Source";
		lightSource.dropDown.pData = &gCurrentLightSource;
		lightSource.dropDown.numNames = MAX_LIGHT_SOURCES;
		lightSource.dropDown.pNames = gLightSourceNames;
		lightSource.dynamic = false;
		AddSubComponent(&gGuiWindow, &lightSource);

		SubComponent lightColor{};
		lightColor.type = SUB_COMPONENT_TYPE_SLIDER_FLOAT3;
		lightColor.sliderFloat3.pLabel = "LightColor";
		lightColor.sliderFloat3.min = vec3(0.0f, 0.0f, 0.0f);
		lightColor.sliderFloat3.max = vec3(1.0f, 1.0f, 1.0f);
		lightColor.sliderFloat3.stepRate = vec3(0.1f, 0.1f, 0.1f);
		lightColor.sliderFloat3.pData = &gLightColor;
		lightColor.sliderFloat3.format = "%.3f";
		lightColor.dynamic = false;
		AddSubComponent(&gGuiWindow, &lightColor);

		SubComponent bias{};
		bias.type = SUB_COMPONENT_TYPE_SLIDER_FLOAT;
		bias.sliderFloat.pLabel = "Shadow Bias";
		bias.sliderFloat.min = 0.0f;
		bias.sliderFloat.max = 1.0f;
		bias.sliderFloat.stepRate = 0.01f;
		bias.sliderFloat.pData = &gShadowBias;
		bias.sliderFloat.format = "%.3f";
		bias.dynamic = false;
		AddSubComponent(&gGuiWindow, &bias);

		SubComponent pLPosition{};
		pLPosition.type = SUB_COMPONENT_TYPE_SLIDER_FLOAT4;
		pLPosition.sliderFloat4.pLabel = "Point Light Position";
		pLPosition.sliderFloat4.min = vec4(-5.0f, -5.0f, -5.0f, 1.0f);
		pLPosition.sliderFloat4.max = vec4(5.0f, 5.0f, 5.0f, 1.0f);
		pLPosition.sliderFloat4.stepRate = vec4(0.1f, 0.1f, 0.1f, 0.0f);
		pLPosition.sliderFloat4.pData = &gPointLight.position;
		pLPosition.sliderFloat4.format = "%.3f";
		pLPosition.dynamic = false;
		AddSubComponent(&gGuiWindow, &pLPosition);

		gInnerCutoffSc.type = SUB_COMPONENT_TYPE_SLIDER_FLOAT;
		gInnerCutoffSc.sliderFloat.pLabel = "Inner Cutoff";
		gInnerCutoffSc.sliderFloat.min = 0.0f;
		gInnerCutoffSc.sliderFloat.max = 90.0f;
		gInnerCutoffSc.sliderFloat.stepRate = 1.0f;
		gInnerCutoffSc.sliderFloat.pData = &gInnerCutoffAngle;
		gInnerCutoffSc.sliderFloat.format = "%.3f degrees";
		gInnerCutoffSc.dynamic = true;
		gInnerCutoffSc.show = false;
		AddSubComponent(&gGuiWindow, &gInnerCutoffSc);

		gOuterCutoffSc.type = SUB_COMPONENT_TYPE_SLIDER_FLOAT;
		gOuterCutoffSc.sliderFloat.pLabel = "Outer Cutoff";
		gOuterCutoffSc.sliderFloat.min = 0.0f;
		gOuterCutoffSc.sliderFloat.max = 90.0f;
		gOuterCutoffSc.sliderFloat.stepRate = 1.0f;
		gOuterCutoffSc.sliderFloat.pData = &gOuterCutoffAngle;
		gOuterCutoffSc.sliderFloat.format = "%.3f degrees";
		gOuterCutoffSc.dynamic = true;
		gOuterCutoffSc.show = false;
		AddSubComponent(&gGuiWindow, &gOuterCutoffSc);

		SubComponent lightSourceCheckbox{};
		lightSourceCheckbox.type = SUB_COMPONENT_TYPE_CHECKBOX;
		lightSourceCheckbox.checkBox.pLabel = "Show Light Sources";
		lightSourceCheckbox.checkBox.pData = &gShowLightSources;
		lightSourceCheckbox.dynamic = false;
		AddSubComponent(&gGuiWindow, &lightSourceCheckbox);

		SubComponent rotate{};
		rotate.type = SUB_COMPONENT_TYPE_CHECKBOX;
		rotate.checkBox.pLabel = "Rotate";
		rotate.checkBox.pData = &gRotate;
		rotate.dynamic = false;
		AddSubComponent(&gGuiWindow, &rotate);

		SubComponent shadowDebug{};
		shadowDebug.type = SUB_COMPONENT_TYPE_CHECKBOX;
		shadowDebug.checkBox.pLabel = "Show Shadow Debug";
		shadowDebug.checkBox.pData = &gShowShadowDebug;
		shadowDebug.dynamic = false;
		AddSubComponent(&gGuiWindow, &shadowDebug);

		SubComponent debugIndex{};
		debugIndex.type = SUB_COMPONENT_TYPE_SLIDER_INT;
		debugIndex.sliderInt.pLabel = "Point Light Debug Index";
		debugIndex.sliderInt.min = 0;
		debugIndex.sliderInt.max = 5;
		debugIndex.sliderInt.stepRate = 1;
		debugIndex.sliderInt.pData = &gDebugIndex;
		debugIndex.sliderInt.format = "%d";
		debugIndex.dynamic = false;
		AddSubComponent(&gGuiWindow, &debugIndex);
	}

	void Exit() override
	{
		WaitQueueIdle(&gRenderer, &gGraphicsQueue);

		DestroyMainComponent(&gGuiWindow);

		DestroyShape(&gVertices, &gIndices);

		DestroyUI(&gRenderer);
		DestroyDescriptorSet(&gDescriptorSetPerNone);
		DestroyDescriptorSet(&gDescriptorSetPerFrame);

		DestroySampler(&gRenderer, &gSampler);

		for (uint32_t i = 0; i < gNumFrames; ++i)
		{
			DestroyBuffer(&gRenderer, &gObjectUniformBuffers[i]);
			DestroyBuffer(&gRenderer, &gCameraUniformBuffer[i]);
			DestroyBuffer(&gRenderer, &gLightSourceUniformBuffer[i]);
			DestroyBuffer(&gRenderer, &gPointLightUniformBuffer[i]);
			DestroyBuffer(&gRenderer, &gDirectionalLightUniformBuffer[i]);
			DestroyBuffer(&gRenderer, &gSpotlightUniformBuffer[i]);

			DestroySemaphore(&gRenderer, &gImageAvailableSemaphores[i]);
			DestroyCommandBuffer(&gRenderer, &gGraphicsCommandBuffers[i]);
		}

		DestroyPipeline(&gRenderer, &gObjectPipeline);
		DestroyPipeline(&gRenderer, &gShadowMapPipeline);
		DestroyPipeline(&gRenderer, &gLightSourcePipeline);
		DestroyPipeline(&gRenderer, &gShadowDebugPipeline);

		DestroyRootSignature(&gRenderer, &gGraphicsRootSignature);

		DestroyBuffer(&gRenderer, &gIndexBuffer);
		DestroyBuffer(&gRenderer, &gVertexBuffer);

		DestroyShader(&gRenderer, &gObjectVS);
		DestroyShader(&gRenderer, &gLightSourceVS);
		DestroyShader(&gRenderer, &gShadowMapVS);
		DestroyShader(&gRenderer, &gShadowDebugVS);

		DestroyShader(&gRenderer, &gObjectPS);
		DestroyShader(&gRenderer, &gShadowMapPS);
		DestroyShader(&gRenderer, &gLightSourcePS);
		DestroyShader(&gRenderer, &gShadowDebugPS);


		for (uint32_t i = 0; i < 6; ++i)
		{
			DestroyRenderTarget(&gRenderer, &gShadowMapPL[i]);
		}
		DestroyRenderTarget(&gRenderer, &gShadowMap);
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
			LookAt(&gCamera, vec3(0.0f, -3.0f, -6.0f), vec3(0.0f, -3.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f));
		if (CheckKeyDown('L'))
			RotateCamera(&gCamera, quat::MakeRotation(turnSpeed2 * deltaTime, vec3(0.0f, 1.0f, 0.0f)));
		if (CheckKeyDown('J'))
			RotateCamera(&gCamera, quat::MakeRotation(-turnSpeed2 * deltaTime, vec3(0.0f, 1.0f, 0.0f)));
		if (CheckKeyDown('I'))
			RotateCamera(&gCamera, quat::MakeRotation(turnSpeed2 * deltaTime, gCamera.right));
		if (CheckKeyDown('K'))
			RotateCamera(&gCamera, quat::MakeRotation(-turnSpeed2 * deltaTime, gCamera.right));
		if (CheckKeyDown(VK_ESCAPE))
		{
			extern bool gQuit;
			gQuit = true;
		}

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

		uint32_t newWidth = GetWidth(pWindow);
		uint32_t newHeight = GetHeight(pWindow);

		gSwapChain.info.width = newWidth;
		gSwapChain.info.height = newHeight;

		SwapChainResize(&gRenderer, &gSwapChain.info, &gSwapChain);

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

		UpdateViewMatrix(&gDLCamera);
		UpdateOrthographicProjectionMatrix(&gDLCamera);

		gCameraData.cameraView = Transpose(gCamera.viewMat);
		gCameraData.cameraPos = vec4(gCamera.position.GetX(), gCamera.position.GetY(), gCamera.position.GetZ(), 1.0f);
		gCameraData.cameraProjection = Transpose(gCamera.perspectiveProjMat);

		if (gRotate)
		{
			gAngle += 45.0f * deltaTime;
			if (gAngle >= 360.0f)
				gAngle = 0.0f;
		}
		
		//Right wall
		gObjectData.objectModel[0] = mat4::Scale(10.0f, 10.0f, 1.0f) * mat4::RotY(90.0f) * mat4::Translate(5.0f, 0.0f, 0.0f);
		gObjectData.objectInverseModel[0] = Inverse(gObjectData.objectModel[0]);

		//Left wall
		gObjectData.objectModel[1] = mat4::Scale(10.0f, 10.0f, 1.0f) * mat4::RotY(-90.0f) * mat4::Translate(-5.0f, 0.0f, 0.0f);
		gObjectData.objectInverseModel[1] = Inverse(gObjectData.objectModel[1]);

		//Top wall
		gObjectData.objectModel[2] = mat4::Scale(10.0f, 10.0f, 1.0f) * mat4::RotX(-90.0f) * mat4::Translate(0.0f, 5.0f, 0.0f);
		gObjectData.objectInverseModel[2] = Inverse(gObjectData.objectModel[2]);

		//Bottom wall
		gObjectData.objectModel[3] = mat4::Scale(10.0f, 10.0f, 1.0f) * mat4::RotX(90.0f) * mat4::Translate(0.0f, -5.0f, 0.0f);
		gObjectData.objectInverseModel[3] = Inverse(gObjectData.objectModel[3]);

		//Back wall
		gObjectData.objectModel[4] = mat4::Scale(10.0f, 10.0f, 1.0f) * mat4::Translate(0.0f, 0.0f, 5.0f);
		gObjectData.objectInverseModel[4] = Inverse(gObjectData.objectModel[4]);

		//Sphere
		gObjectData.objectModel[5] = mat4::Scale(1.0f, 1.0f, 1.0f) * mat4::Translate(-3.0f, -4.0f, 0.0f);
		gObjectData.objectInverseModel[5] = Inverse(gObjectData.objectModel[5]);

		//Cylinder
		gObjectData.objectModel[6] = mat4::Scale(1.0f, 3.0f, 1.0f) * mat4::Translate(3.0f, -3.2f, 2.0f);
		gObjectData.objectInverseModel[6] = Inverse(gObjectData.objectModel[6]);

		for (uint32_t i = 0; i < NUM_OBJECTS; ++i)
		{
			gObjectData.objectModel[i] = Transpose(gObjectData.objectModel[i]);
		}

		vec4 lightColor = vec4(gLightColor.GetX(), gLightColor.GetY(), gLightColor.GetZ(), 1.0f);

		//Point light
		//gPointLight.position = vec4(-1.0f, -4.0f, 0.0f, 1.0f);
		gPointLight.color = lightColor;

		//Directional light
		gDirectionalLight.direction = vec4(0.0f, -1.0f, 0.0f, 0.0f);
		gDirectionalLight.color = lightColor;

		//Spotlight
		gSpotlight.position = vec4(-2.0f, 0.0f, 0.0f, 1.0f);
		gSpotlight.direction = vec4(1.0f, 0.0f, 0.0f, 0.0f);
		gSpotlight.color = lightColor;
		gSpotlight.innerCutoff = cos(gInnerCutoffAngle * PI / 180.0f);
		gSpotlight.outerCutoff = cos(gOuterCutoffAngle * PI / 180.0f);

		if (gCurrentLightSource == POINT_LIGHT)
		{
			vec3 position = vec3(gPointLight.position.GetX(), gPointLight.position.GetY(), gPointLight.position.GetZ());

			LookAt(&gPLCamera[0], position, position + vec3(1.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f)); //right
			LookAt(&gPLCamera[1], position, position + vec3(-1.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f)); //left

			LookAt(&gPLCamera[2], position, position + vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f)); //top
			LookAt(&gPLCamera[3], position, position + vec3(0.0f, -1.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f)); //bottom

			LookAt(&gPLCamera[4], position, position + vec3(0.0f, 0.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f)); //front
			LookAt(&gPLCamera[5], position, position + vec3(0.0f, 0.0f, -1.0f), vec3(0.0f, 1.0f, 0.0f)); //back

			for (uint32_t i = 0; i < 6; ++i)
			{
				gLightSourceData.lightSourceModel[i + 1] = Transpose(mat4::Scale(0.1f, 0.1f, 0.1f) *
					mat4::Translate(gPointLight.position.GetX(), gPointLight.position.GetY(), gPointLight.position.GetZ()));

				gPLCamera[i].position = position;
				gPLCamera[i].aspectRatio = (float)gShadowWidth / gShadowHeight;
				UpdateViewMatrix(&gPLCamera[i]);
				UpdatePerspectiveProjectionMatrix(&gPLCamera[i]);

				gLightSourceData.lightSourceView[i + 1] = Transpose(gPLCamera[i].viewMat);
				gLightSourceData.lightSourceProjection[i + 1] = Transpose(gPLCamera[i].perspectiveProjMat);
				gLightSourceData.lightPosition[i + 1] = gPointLight.position;
			}
		}
		else if (gCurrentLightSource == DIRECTIONAL_LIGHT)
		{
			gLightSourceData.lightSourceModel[0] = Transpose(mat4::Scale(0.1f, 0.1f, 0.1f) *
				mat4::Translate(gDLCamera.position.GetX(), gDLCamera.position.GetY(), gDLCamera.position.GetZ()));

			gLightSourceData.lightSourceView[0] = Transpose(gDLCamera.viewMat);
			gLightSourceData.lightSourceProjection[0] = Transpose(gDLCamera.orthographicProjMat);
			gLightSourceData.lightPosition[0] = vec4(gDLCamera.position.GetX(), gDLCamera.position.GetY(), gDLCamera.position.GetZ(), 1.0f);
		}
		else //SPOTLIGHT
		{
			gLightSourceData.lightSourceModel[7] = Transpose(mat4::Scale(0.1f, 0.1f, 0.1f) *
				mat4::Translate(gSpotlight.position.GetX(), gSpotlight.position.GetY(), gSpotlight.position.GetZ()));
		}

		gConstants.currentLightSource = gCurrentLightSource;
		gConstants.shadowBias = gShadowBias;

		if (gCurrentLightSource == SPOTLIGHT)
		{
			gInnerCutoffSc.show = true;
			gOuterCutoffSc.show = true;
		}
		else
		{
			gInnerCutoffSc.show = false;
			gOuterCutoffSc.show = false;
		}
	}

	void Draw() override
	{
		CommandBuffer* pCommandBuffer = &gGraphicsCommandBuffers[gCurrentFrame];
		WaitForFence(&gRenderer, &pCommandBuffer->fence);

		void* data = nullptr;

		MapMemory(&gRenderer, &gCameraUniformBuffer[gCurrentFrame], &data);
		memcpy(data, &gCameraData, sizeof(CameraData));
		UnmapMemory(&gRenderer, &gCameraUniformBuffer[gCurrentFrame]);

		MapMemory(&gRenderer, &gPointLightUniformBuffer[gCurrentFrame], &data);
		memcpy(data, &gPointLight, sizeof(PointLight));
		UnmapMemory(&gRenderer, &gPointLightUniformBuffer[gCurrentFrame]);

		MapMemory(&gRenderer, &gDirectionalLightUniformBuffer[gCurrentFrame], &data);
		memcpy(data, &gDirectionalLight, sizeof(DirectionalLight));
		UnmapMemory(&gRenderer, &gDirectionalLightUniformBuffer[gCurrentFrame]);

		MapMemory(&gRenderer, &gSpotlightUniformBuffer[gCurrentFrame], &data);
		memcpy(data, &gSpotlight, sizeof(Spotlight));
		UnmapMemory(&gRenderer, &gSpotlightUniformBuffer[gCurrentFrame]);

		MapMemory(&gRenderer, &gLightSourceUniformBuffer[gCurrentFrame], &data);
		memcpy(data, &gLightSourceData, sizeof(LightSourceData));
		UnmapMemory(&gRenderer, &gLightSourceUniformBuffer[gCurrentFrame]);

		MapMemory(&gRenderer, &gObjectUniformBuffers[gCurrentFrame], &data);
		memcpy(data, &gObjectData, sizeof(ObjectData));
		UnmapMemory(&gRenderer, &gObjectUniformBuffers[gCurrentFrame]);

		uint32_t imageIndex = 0;
		AcquireNextImage(&gRenderer, &gSwapChain, &gImageAvailableSemaphores[gCurrentFrame], &imageIndex);

		RenderTarget* pRenderTarget = &gSwapChain.pRenderTargets[imageIndex];

		ResetCommandBuffer(&gRenderer, pCommandBuffer);

		BeginCommandBuffer(pCommandBuffer);

		BindVertexBuffer(pCommandBuffer, sizeof(Vertex), 0, 0, &gVertexBuffer);
		BindIndexBuffer(pCommandBuffer, 0, INDEX_TYPE_UINT32, &gIndexBuffer);

		//DL Shadow Map
		if (gCurrentLightSource == DIRECTIONAL_LIGHT)
		{
			BarrierInfo barrierInfo[1]{};
			barrierInfo[0].type = BARRIER_TYPE_RENDER_TARGET;
			barrierInfo[0].pRenderTarget = &gShadowMap;
			barrierInfo[0].currentState = RESOURCE_STATE_ALL_SHADER_RESOURCE;
			barrierInfo[0].newState = RESOURCE_STATE_DEPTH_WRITE;
			ResourceBarrier(pCommandBuffer, 1, barrierInfo);

			ViewportInfo viewportInfo{};
			viewportInfo.x = 0.0f;
			viewportInfo.y = 0.0f;
			viewportInfo.width = gShadowWidth;
			viewportInfo.height = gShadowHeight;
			viewportInfo.minDepth = 0.0f;
			viewportInfo.maxDepth = 1.0f;
			SetViewport(pCommandBuffer, &viewportInfo);

			ScissorInfo scissorInfo{};
			scissorInfo.x = 0.0f;
			scissorInfo.y = 0.0f;
			scissorInfo.width = gShadowWidth;
			scissorInfo.height = gShadowHeight;
			SetScissor(pCommandBuffer, &scissorInfo);

			BindRenderTargetInfo renderTargetInfo{};
			renderTargetInfo.pRenderTarget = nullptr;
			renderTargetInfo.renderTargetLoadOp = LOAD_OP_CLEAR;
			renderTargetInfo.renderTargetStoreOp = STORE_OP_STORE;
			renderTargetInfo.pDepthTarget = &gShadowMap;
			renderTargetInfo.depthTargetLoadOp = LOAD_OP_CLEAR;
			renderTargetInfo.depthTargetStoreOp = STORE_OP_DONT_CARE;
			BindRenderTarget(pCommandBuffer, &renderTargetInfo);

			BindPipeline(pCommandBuffer, &gShadowMapPipeline);
			BindDescriptorSet(pCommandBuffer, 0, 0, &gDescriptorSetPerNone);
			BindDescriptorSet(pCommandBuffer, gCurrentFrame, 1, &gDescriptorSetPerFrame);
			gConstants.lightIndex = 0;

			gConstants.objectIndex = 5;
			BindRootConstants(pCommandBuffer, 5, sizeof(RootConstants), &gConstants, 0);
			DrawIndexedInstanced(pCommandBuffer, gIndexCounts[SPHERE], 1, gIndexOffsets[SPHERE], gVertexOffsets[SPHERE], 0);

			gConstants.objectIndex = 6;
			BindRootConstants(pCommandBuffer, 5, sizeof(RootConstants), &gConstants, 0);
			BindDescriptorSet(pCommandBuffer, gCurrentFrame, 1, &gDescriptorSetPerFrame);
			DrawIndexedInstanced(pCommandBuffer, gIndexCounts[CYLINDER], 1, gIndexOffsets[CYLINDER], gVertexOffsets[CYLINDER], 0);

			BindRenderTarget(pCommandBuffer, nullptr);

			barrierInfo[0].type = BARRIER_TYPE_RENDER_TARGET;
			barrierInfo[0].pRenderTarget = &gShadowMap;
			barrierInfo[0].currentState = RESOURCE_STATE_DEPTH_WRITE;
			barrierInfo[0].newState = RESOURCE_STATE_ALL_SHADER_RESOURCE;
			ResourceBarrier(pCommandBuffer, 1, barrierInfo);

		}
		else if(gCurrentLightSource == POINT_LIGHT) //Point light shadow maps
		{
			BarrierInfo barrierInfo[6]{};
			for (uint32_t i = 0; i < 6; ++i)
			{
				barrierInfo[i].type = BARRIER_TYPE_RENDER_TARGET;
				barrierInfo[i].pRenderTarget = &gShadowMapPL[i];
				barrierInfo[i].currentState = RESOURCE_STATE_ALL_SHADER_RESOURCE;
				barrierInfo[i].newState = RESOURCE_STATE_DEPTH_WRITE;
			}
			ResourceBarrier(pCommandBuffer, 6, barrierInfo);

			ViewportInfo viewportInfo{};
			viewportInfo.x = 0.0f;
			viewportInfo.y = 0.0f;
			viewportInfo.width = gShadowWidth;
			viewportInfo.height = gShadowHeight;
			viewportInfo.minDepth = 0.0f;
			viewportInfo.maxDepth = 1.0f;
			SetViewport(pCommandBuffer, &viewportInfo);

			ScissorInfo scissorInfo{};
			scissorInfo.x = 0.0f;
			scissorInfo.y = 0.0f;
			scissorInfo.width = gShadowWidth;
			scissorInfo.height = gShadowHeight;
			SetScissor(pCommandBuffer, &scissorInfo);

			for (uint32_t i = 0; i < 6; ++i)
			{
				BindRenderTargetInfo renderTargetInfo{};
				renderTargetInfo.pRenderTarget = nullptr;
				renderTargetInfo.renderTargetLoadOp = LOAD_OP_CLEAR;
				renderTargetInfo.renderTargetStoreOp = STORE_OP_STORE;
				renderTargetInfo.pDepthTarget = &gShadowMapPL[i];
				renderTargetInfo.depthTargetLoadOp = LOAD_OP_CLEAR;
				renderTargetInfo.depthTargetStoreOp = STORE_OP_DONT_CARE;
				BindRenderTarget(pCommandBuffer, &renderTargetInfo);

				BindPipeline(pCommandBuffer, &gShadowMapPipeline);
				BindDescriptorSet(pCommandBuffer, 0, 0, &gDescriptorSetPerNone);
				BindDescriptorSet(pCommandBuffer, gCurrentFrame, 1, &gDescriptorSetPerFrame);
				gConstants.lightIndex = i + 1;

				for (uint32_t i = 0; i < 5; ++i)
				{
					gConstants.objectIndex = i;
					BindRootConstants(pCommandBuffer, 5, sizeof(RootConstants), &gConstants, 0);
					DrawIndexedInstanced(pCommandBuffer, gIndexCounts[WALL], 1, gIndexOffsets[WALL], gVertexOffsets[WALL], 0);
				}

				gConstants.objectIndex = 5;
				BindRootConstants(pCommandBuffer, 5, sizeof(RootConstants), &gConstants, 0);
				DrawIndexedInstanced(pCommandBuffer, gIndexCounts[SPHERE], 1, gIndexOffsets[SPHERE], gVertexOffsets[SPHERE], 0);

				gConstants.objectIndex = 6;
				BindRootConstants(pCommandBuffer, 5, sizeof(RootConstants), &gConstants, 0);
				DrawIndexedInstanced(pCommandBuffer, gIndexCounts[CYLINDER], 1, gIndexOffsets[CYLINDER], gVertexOffsets[CYLINDER], 0);
			}

			BindRenderTarget(pCommandBuffer, nullptr);

			for (uint32_t i = 0; i < 6; ++i)
			{
				barrierInfo[i].type = BARRIER_TYPE_RENDER_TARGET;
				barrierInfo[i].pRenderTarget = &gShadowMapPL[i];
				barrierInfo[i].currentState = RESOURCE_STATE_DEPTH_WRITE;
				barrierInfo[i].newState = RESOURCE_STATE_ALL_SHADER_RESOURCE;
			}
			ResourceBarrier(pCommandBuffer, 6, barrierInfo);
		}

		BarrierInfo barrierInfo[1];
		barrierInfo[0].type = BARRIER_TYPE_RENDER_TARGET;
		barrierInfo[0].pRenderTarget = pRenderTarget;
		barrierInfo[0].currentState = RESOURCE_STATE_PRESENT;
		barrierInfo[0].newState = RESOURCE_STATE_RENDER_TARGET;

		ResourceBarrier(pCommandBuffer, 1, barrierInfo);

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

		//Draw Objects
		if(gShowShadowDebug == false)
		{
			BindPipeline(pCommandBuffer, &gObjectPipeline);
			BindDescriptorSet(pCommandBuffer, 0, 0, &gDescriptorSetPerNone);
			BindDescriptorSet(pCommandBuffer, gCurrentFrame, 1, &gDescriptorSetPerFrame);

			for (uint32_t i = 0; i < 5; ++i)
			{
				gConstants.objectIndex = i;
				BindRootConstants(pCommandBuffer, 5, sizeof(RootConstants), &gConstants, 0);
				DrawIndexedInstanced(pCommandBuffer, gIndexCounts[WALL], 1, gIndexOffsets[WALL], gVertexOffsets[WALL], 0);
			}

			gConstants.objectIndex = 5;
			BindRootConstants(pCommandBuffer, 5, sizeof(RootConstants), &gConstants, 0);
			DrawIndexedInstanced(pCommandBuffer, gIndexCounts[SPHERE], 1, gIndexOffsets[SPHERE], gVertexOffsets[SPHERE], 0);

			gConstants.objectIndex = 6;
			BindRootConstants(pCommandBuffer, 5, sizeof(RootConstants), &gConstants, 0);
			DrawIndexedInstanced(pCommandBuffer, gIndexCounts[CYLINDER], 1, gIndexOffsets[CYLINDER], gVertexOffsets[CYLINDER], 0);

			if (gShowLightSources)
			{
				//Draw Light Source
				BindPipeline(pCommandBuffer, &gLightSourcePipeline);
				BindDescriptorSet(pCommandBuffer, gCurrentFrame, 1, &gDescriptorSetPerFrame);
				DrawIndexedInstanced(pCommandBuffer, gIndexCounts[SPHERE], 1, gIndexOffsets[SPHERE], gVertexOffsets[SPHERE], 0);
			}
		}
		else
		{

			BindPipeline(pCommandBuffer, &gShadowDebugPipeline);
			BindDescriptorSet(pCommandBuffer, 0, 0, &gDescriptorSetPerNone);
			BindDescriptorSet(pCommandBuffer, gCurrentFrame, 1, &gDescriptorSetPerFrame);

			gConstants.debugIndex = gDebugIndex;
			BindRootConstants(pCommandBuffer, 5, sizeof(RootConstants), &gConstants, 0);
			DrawInstanced(pCommandBuffer, 3, 1, 0, 0);
		}

		RenderUI(pCommandBuffer);

		BindRenderTarget(pCommandBuffer, nullptr);

		barrierInfo[0].type = BARRIER_TYPE_RENDER_TARGET;
		barrierInfo[0].pRenderTarget = pRenderTarget;
		barrierInfo[0].currentState = RESOURCE_STATE_RENDER_TARGET;
		barrierInfo[0].newState = RESOURCE_STATE_PRESENT;
		ResourceBarrier(pCommandBuffer, 1, barrierInfo);

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
	Shadows shadows;
	shadows.appName = "Shadows";
	return WindowsMain(&shadows);
};
