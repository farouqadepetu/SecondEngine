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

RenderTarget gShadowMap;
RenderTarget gShadowMapPL[6];

RootSignature gGraphicsRootSignature;

Shader gObjectVS;
Shader gLightSourceVS;
Shader gShadowMapVS;
Shader gSkyboxVS;

Shader gPhongPS;
Shader gPbrPS;
Shader gLightSourcePS;
Shader gShadowMapPS;
Shader gSkyboxPS;

Pipeline gPhongPipeline;
Pipeline gPbrPipeline;
Pipeline gLightSourcePipeline;
Pipeline gShadowMapPipeline;
Pipeline gSkyboxPipeline;

const uint32_t gNumFrames = 2;
const uint32_t gShadowWidth = 1024;
const uint32_t gShadowHeight = 1024;

Semaphore gImageAvailableSemaphores[gNumFrames];
CommandBuffer gGraphicsCommandBuffers[gNumFrames];

Buffer gVertexBuffer;
Buffer gIndexBuffer;

Texture gWoodColor;
Texture gWoodRoughness;
Texture gWoodNormal;
Texture gSkyboxTexture;
Sampler gSampler;

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

struct LightSourceData
{
	mat4 lightSourceModel;
	mat4 lightSourceView;
	mat4 lightSourceProjection;
};

struct PointLight
{
	vec4 position;
	vec4 color;
};

struct DirectionalLight
{
	vec4 direction;
	vec4 color;
};

struct Spotlight
{
	vec4 position;
	vec4 direction;
	vec4 color;
	float innerCutoff;
	float outerCutoff;
};

struct PhongMaterial
{
	vec4 diffuse;
	vec4 specular;
	float ambientIntensity;
	float shininess;
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
	PLANE,
	BOX,
	SPHERE,
	HEMI_SPHERE,
	TORUS,
	CYLINDER,
	MAX_SHAPES
};

enum
{
	PBR,
	PHONG,
	MAX_SHADING
};

enum
{
	DIRECTIONAL_LIGHT,
	POINT_LIGHT,
	SPOTLIGHT,
	MAX_LIGHT_SOURCES
};

struct ObjectData
{
	mat4 objectModel[MAX_SHAPES];
	mat4 objectInverseModel[MAX_SHAPES];
};

PointLight gPointLight;
Buffer gPointLightUniformBuffer[gNumFrames];

DirectionalLight gDirectionalLight;
Buffer gDirectionalLightUniformBuffer[gNumFrames];

Spotlight gSpotlight;
Buffer gSpotlightUniformBuffer[gNumFrames];

LightSourceData gLightSourceData;
LightSourceData gLightSourceDataPL[6];
Buffer gLightSourceUniformBuffer[gNumFrames * 7];

CameraData gCameraData;
Buffer gCameraUniformBuffer[gNumFrames];

ObjectData gShapesData;
Buffer gShapesUniformBuffers[gNumFrames];

ObjectData gSkyBoxData;
Buffer gSkyboxUniformBuffer[gNumFrames];

CameraData gSkyboxCameraData;
Buffer gSkyboxCameraBuffer[gNumFrames];

uint32_t gVertexCounts[MAX_SHAPES];
uint32_t gVertexOffsets[MAX_SHAPES];
uint32_t gIndexOffsets[MAX_SHAPES];
uint32_t gIndexCounts[MAX_SHAPES];

MainComponent gGuiWindow;

const char* gLightSourceNames[] =
{
	"DIRECTIONAL",
	"POINT",
	"SPOTLIGHT",
};
uint32_t gCurrentLightSource = 0;

const char* gShadingNames[] = { "PBR", "PHONG" };
uint32_t gCurrentShading = 0;

bool gShowLightSources = false;
bool gRotate = false;

float gInnerCutoffAngle = 2.0f;
float gOuterCutoffAngle = 5.0f;

struct RootConstants
{
	uint32_t currentLightSource;
	uint32_t shape;
};

RootConstants gConstants;

float gAngle = 0.0f;

vec3 gLightColor = vec3(1.0f, 1.0f, 1.0f);

SubComponent gInnerCutoffSc{};
SubComponent gOuterCutoffSc{};

class Lightning : public App
{
public:
	void Init() override
	{
		InitSE();

		InitRenderer(&gRenderer, "Lightning");

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

		shaderInfo.filename = "skybox.vert";
		shaderInfo.type = SHADER_TYPE_VERTEX;
		CreateShader(&gRenderer, &shaderInfo, &gSkyboxVS);

		shaderInfo.filename = "phong.frag";
		shaderInfo.type = SHADER_TYPE_PIXEL;
		CreateShader(&gRenderer, &shaderInfo, &gPhongPS);

		shaderInfo.filename = "pbr.frag";
		shaderInfo.type = SHADER_TYPE_PIXEL;
		CreateShader(&gRenderer, &shaderInfo, &gPbrPS);

		shaderInfo.filename = "lightSource.frag";
		shaderInfo.type = SHADER_TYPE_PIXEL;
		CreateShader(&gRenderer, &shaderInfo, &gLightSourcePS);

		shaderInfo.filename = "shadowMap.frag";
		shaderInfo.type = SHADER_TYPE_PIXEL;
		CreateShader(&gRenderer, &shaderInfo, &gShadowMapPS);

		shaderInfo.filename = "skybox.frag";
		shaderInfo.type = SHADER_TYPE_PIXEL;
		CreateShader(&gRenderer, &shaderInfo, &gSkyboxPS);

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

		RootParameterInfo rootParameterInfos[12]{};

		//PerFrame
		rootParameterInfos[0].binding = 0;
		rootParameterInfos[0].baseRegister = 0;
		rootParameterInfos[0].registerSpace = 0;
		rootParameterInfos[0].numDescriptors = 1;
		rootParameterInfos[0].stages = STAGE_VERTEX | STAGE_PIXEL;
		rootParameterInfos[0].type = DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		rootParameterInfos[0].updateFrequency = UPDATE_FREQUENCY_PER_FRAME;

		//PerObject
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
		rootParameterInfos[2].updateFrequency = UPDATE_FREQUENCY_PER_DRAW;

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

		//Wood Color texture
		rootParameterInfos[6].binding = 0;
		rootParameterInfos[6].baseRegister = 0;
		rootParameterInfos[6].registerSpace = 0;
		rootParameterInfos[6].numDescriptors = 1;
		rootParameterInfos[6].stages = STAGE_PIXEL;
		rootParameterInfos[6].type = DESCRIPTOR_TYPE_TEXTURE;
		rootParameterInfos[6].updateFrequency = UPDATE_FREQUENCY_PER_NONE;

		//Wood Roughness texture
		rootParameterInfos[7].binding = 1;
		rootParameterInfos[7].baseRegister = 1;
		rootParameterInfos[7].registerSpace = 0;
		rootParameterInfos[7].numDescriptors = 1;
		rootParameterInfos[7].stages = STAGE_PIXEL;
		rootParameterInfos[7].type = DESCRIPTOR_TYPE_TEXTURE;
		rootParameterInfos[7].updateFrequency = UPDATE_FREQUENCY_PER_NONE;

		//Wood Normal texture
		rootParameterInfos[8].binding = 2;
		rootParameterInfos[8].baseRegister = 2;
		rootParameterInfos[8].registerSpace = 0;
		rootParameterInfos[8].numDescriptors = 1;
		rootParameterInfos[8].stages = STAGE_PIXEL;
		rootParameterInfos[8].type = DESCRIPTOR_TYPE_TEXTURE;
		rootParameterInfos[8].updateFrequency = UPDATE_FREQUENCY_PER_NONE;

		//Shadow map texture
		rootParameterInfos[9].binding = 3;
		rootParameterInfos[9].baseRegister = 3;
		rootParameterInfos[9].registerSpace = 0;
		rootParameterInfos[9].numDescriptors = 1;
		rootParameterInfos[9].stages = STAGE_PIXEL;
		rootParameterInfos[9].type = DESCRIPTOR_TYPE_TEXTURE;
		rootParameterInfos[9].updateFrequency = UPDATE_FREQUENCY_PER_NONE;

		//Skybox texture
		rootParameterInfos[10].binding = 4;
		rootParameterInfos[10].baseRegister = 4;
		rootParameterInfos[10].registerSpace = 0;
		rootParameterInfos[10].numDescriptors = 1;
		rootParameterInfos[10].stages = STAGE_PIXEL;
		rootParameterInfos[10].type = DESCRIPTOR_TYPE_TEXTURE;
		rootParameterInfos[10].updateFrequency = UPDATE_FREQUENCY_PER_NONE;

		//Sampler
		rootParameterInfos[11].binding = 5;
		rootParameterInfos[11].baseRegister = 0;
		rootParameterInfos[11].registerSpace = 0;
		rootParameterInfos[11].numDescriptors = 1;
		rootParameterInfos[11].stages = STAGE_PIXEL;
		rootParameterInfos[11].type = DESCRIPTOR_TYPE_SAMPLER;
		rootParameterInfos[11].updateFrequency = UPDATE_FREQUENCY_PER_NONE;

		RootConstantsInfo rootConstantInfo{};
		rootConstantInfo.numValues = 2;
		rootConstantInfo.baseRegister = 6;
		rootConstantInfo.registerSpace = 0;
		rootConstantInfo.stride = sizeof(RootConstants);
		rootConstantInfo.stages = STAGE_VERTEX | STAGE_PIXEL;

		RootSignatureInfo graphicsRootSignatureInfo{};
		graphicsRootSignatureInfo.pRootParameterInfos = rootParameterInfos;
		graphicsRootSignatureInfo.numRootParameterInfos = 12;
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
		graphicsPipelineInfo.pPixelShader = &gPhongPS;
		graphicsPipelineInfo.numRenderTargets = 1;
		graphicsPipelineInfo.renderTargetFormat[0] = gSwapChain.pRenderTargets[0].info.format;
		graphicsPipelineInfo.depthInfo.depthTestEnable = true;
		graphicsPipelineInfo.depthInfo.depthWriteEnable = true;
		graphicsPipelineInfo.depthInfo.depthFunction = DEPTH_FUNCTION_LESS;
		graphicsPipelineInfo.depthFormat = gDepthBuffer.info.format;
		graphicsPipelineInfo.pVertexInputInfo = &vertexInputInfo;
		graphicsPipelineInfo.pRootSignature = &gGraphicsRootSignature;
		CreatePipeline(&gRenderer, &graphicsPipelineInfo, &gPhongPipeline);

		graphicsPipelineInfo.pVertexShader = &gObjectVS;
		graphicsPipelineInfo.pPixelShader = &gPbrPS;
		CreatePipeline(&gRenderer, &graphicsPipelineInfo, &gPbrPipeline);

		graphicsPipelineInfo.pVertexShader = &gLightSourceVS;
		graphicsPipelineInfo.pPixelShader = &gLightSourcePS;
		CreatePipeline(&gRenderer, &graphicsPipelineInfo, &gLightSourcePipeline);

		graphicsPipelineInfo.pVertexShader = &gShadowMapVS;
		graphicsPipelineInfo.pPixelShader = &gShadowMapPS;
		graphicsPipelineInfo.numRenderTargets = 0;
		CreatePipeline(&gRenderer, &graphicsPipelineInfo, &gShadowMapPipeline);

		graphicsPipelineInfo.rasInfo.cullMode = CULL_MODE_NONE;
		graphicsPipelineInfo.pVertexShader = &gSkyboxVS;
		graphicsPipelineInfo.pPixelShader = &gSkyboxPS;
		graphicsPipelineInfo.numRenderTargets = 1;
		graphicsPipelineInfo.depthInfo.depthFunction = DEPTH_FUNCTION_LESS_OR_EQUAL;
		CreatePipeline(&gRenderer, &graphicsPipelineInfo, &gSkyboxPipeline);

		for (uint32_t i = 0; i < gNumFrames; ++i)
		{
			CreateCommandBuffer(&gRenderer, QUEUE_TYPE_GRAPHICS, &gGraphicsCommandBuffers[i]);
			CreateSemaphore(&gRenderer, &gImageAvailableSemaphores[i]);
		}

		gVertexOffsets[PLANE] = arrlenu(gVertices);
		gIndexOffsets[PLANE] = arrlenu(gIndices);
		CreateQuad(&gVertices, &gIndices, &gVertexCounts[PLANE], & gIndexCounts[PLANE]);

		gVertexOffsets[BOX] = arrlenu(gVertices);
		gIndexOffsets[BOX] = arrlenu(gIndices);
		CreateBox(&gVertices, &gIndices, &gVertexCounts[PLANE], &gIndexCounts[BOX]);

		gVertexOffsets[SPHERE] = arrlenu(gVertices);
		gIndexOffsets[SPHERE] = arrlenu(gIndices);
		CreateSphere(&gVertices, &gIndices, &gVertexCounts[PLANE], &gIndexCounts[SPHERE]);

		gVertexOffsets[HEMI_SPHERE] = arrlenu(gVertices);
		gIndexOffsets[HEMI_SPHERE] = arrlenu(gIndices);
		CreateHemiSphere(&gVertices, &gIndices, &gVertexCounts[PLANE], &gIndexCounts[HEMI_SPHERE], true);

		gVertexOffsets[TORUS] = arrlenu(gVertices);
		gIndexOffsets[TORUS] = arrlenu(gIndices);
		CreateTorus(&gVertices, &gIndices, &gVertexCounts[PLANE], &gIndexCounts[TORUS], 2.0f, 1.0f);

		gVertexOffsets[CYLINDER] = arrlenu(gVertices);
		gIndexOffsets[CYLINDER] = arrlenu(gIndices);
		CreateCylinder(&gVertices, &gIndices, &gVertexCounts[PLANE], &gIndexCounts[CYLINDER], true, true);

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
			ubInfo.type = BUFFER_TYPE_UNIFORM;
			ubInfo.usage = MEMORY_USAGE_CPU_TO_GPU;
			ubInfo.data = nullptr;

			ubInfo.size = sizeof(ObjectData) * MAX_SHAPES;
			CreateBuffer(&gRenderer, &ubInfo, &gShapesUniformBuffers[i]);

			ubInfo.size = sizeof(CameraData);
			CreateBuffer(&gRenderer, &ubInfo, &gCameraUniformBuffer[i]);
			CreateBuffer(&gRenderer, &ubInfo, &gSkyboxCameraBuffer[i]);

			ubInfo.size = sizeof(ObjectData);
			CreateBuffer(&gRenderer, &ubInfo, &gSkyboxUniformBuffer[i]);

			ubInfo.size = sizeof(PointLight);
			CreateBuffer(&gRenderer, &ubInfo, &gPointLightUniformBuffer[i]);

			ubInfo.size = sizeof(DirectionalLight);
			CreateBuffer(&gRenderer, &ubInfo, &gDirectionalLightUniformBuffer[i]);

			ubInfo.size = sizeof(Spotlight);
			CreateBuffer(&gRenderer, &ubInfo, &gSpotlightUniformBuffer[i]);

			ubInfo.size = sizeof(LightSourceData);
			CreateBuffer(&gRenderer, &ubInfo, &gLightSourceUniformBuffer[i]);
		}

		TextureInfo texInfo{};
		texInfo.filename = "Textures/woodColor.dds";
		CreateTexture(&gRenderer, &texInfo, &gWoodColor);

		texInfo.filename = "Textures/woodRoughness.dds";
		CreateTexture(&gRenderer, &texInfo, &gWoodRoughness);

		texInfo.filename = "Textures/woodNormal.dds";
		CreateTexture(&gRenderer, &texInfo, &gWoodNormal);

		texInfo.filename = "Textures/skybox.dds";
		CreateTexture(&gRenderer, &texInfo, &gSkyboxTexture);

		SamplerInfo samplerInfo{};
		samplerInfo.magFilter = FILTER_LINEAR;
		samplerInfo.minFilter = FILTER_LINEAR;
		samplerInfo.u = ADDRESS_MODE_CLAMP_TO_EDGE;
		samplerInfo.v = ADDRESS_MODE_CLAMP_TO_EDGE;
		samplerInfo.w = ADDRESS_MODE_CLAMP_TO_EDGE;
		samplerInfo.mipMapMode = MIPMAP_MODE_LINEAR;
		samplerInfo.maxAnisotropy = 0.0f;
		samplerInfo.mipLoadBias = 0.0f;
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = 0.0f;
		CreateSampler(&gRenderer, &samplerInfo, &gSampler);

		DescriptorSetInfo setInfo{};
		setInfo.pRootSignature = &gGraphicsRootSignature;
		setInfo.numSets = gNumFrames + 2;
		setInfo.updateFrequency = UPDATE_FREQUENCY_PER_FRAME;
		CreateDescriptorSet(&gRenderer, &setInfo, &gDescriptorSetPerFrame);

		setInfo.pRootSignature = &gGraphicsRootSignature;
		setInfo.numSets = 1;
		setInfo.updateFrequency = UPDATE_FREQUENCY_PER_NONE;
		CreateDescriptorSet(&gRenderer, &setInfo, &gDescriptorSetPerNone);

		for (uint32_t i = 0; i < gNumFrames; ++i)
		{
			UpdateDescriptorSetInfo updatePerFrame[6]{};
			updatePerFrame[0].binding = 0;
			updatePerFrame[0].type = UPDATE_TYPE_UNIFORM_BUFFER;
			updatePerFrame[0].pBuffer = &gCameraUniformBuffer[i];

			updatePerFrame[1].binding = 1;
			updatePerFrame[1].type = UPDATE_TYPE_UNIFORM_BUFFER;
			updatePerFrame[1].pBuffer = &gShapesUniformBuffers[i];

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

			updatePerFrame[0].binding = 0;
			updatePerFrame[0].type = UPDATE_TYPE_UNIFORM_BUFFER;
			updatePerFrame[0].pBuffer = &gSkyboxCameraBuffer[i];

			updatePerFrame[1].binding = 1;
			updatePerFrame[1].type = UPDATE_TYPE_UNIFORM_BUFFER;
			updatePerFrame[1].pBuffer = &gSkyboxUniformBuffer[i];
			UpdateDescriptorSet(&gRenderer, &gDescriptorSetPerFrame, gNumFrames + i, 6, updatePerFrame);
		}

		UpdateDescriptorSetInfo updateImageSetInfo[6]{};
		updateImageSetInfo[0].binding = 0;
		updateImageSetInfo[0].type = UPDATE_TYPE_TEXTURE;
		updateImageSetInfo[0].pTexture = &gWoodColor;

		updateImageSetInfo[1].binding = 1;
		updateImageSetInfo[1].type = UPDATE_TYPE_TEXTURE;
		updateImageSetInfo[1].pTexture = &gWoodRoughness;

		updateImageSetInfo[2].binding = 2;
		updateImageSetInfo[2].type = UPDATE_TYPE_TEXTURE;
		updateImageSetInfo[2].pTexture = &gWoodNormal;

		updateImageSetInfo[3].binding = 3;
		updateImageSetInfo[3].type = UPDATE_TYPE_TEXTURE;
		updateImageSetInfo[3].pTexture = &gShadowMap.texture;

		updateImageSetInfo[4].binding = 4;
		updateImageSetInfo[4].type = UPDATE_TYPE_TEXTURE;
		updateImageSetInfo[4].pTexture = &gSkyboxTexture;

		updateImageSetInfo[5].binding = 5;
		updateImageSetInfo[5].type = UPDATE_TYPE_SAMPLER;
		updateImageSetInfo[5].pSampler = &gSampler;
		UpdateDescriptorSet(&gRenderer, &gDescriptorSetPerNone, 0, 6, updateImageSetInfo);
	
		LookAt(&gCamera, vec3(3.0f, 6.0f, -18.0f), vec3(3.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
		gCamera.vFov = 45.0f;
		gCamera.nearP = 1.0f;
		gCamera.farP = 100.0f;

		LookAt(&gDLCamera, vec3(0.0f, 5.0f, -10.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
		gDLCamera.width = 25.0f;
		gDLCamera.height = 20.0f;
		gDLCamera.nearP = 1.0f;
		gDLCamera.farP = 25.0f;

		for (uint32_t i = 0; i < 6; ++i)
		{
			gPLCamera[i].vFov = 90.0f;
			gPLCamera[i].nearP = 1.0f;
			gPLCamera[i].farP = 100.0f;
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
		mcInfo.position = vec2(GetWidth(pWindow) - 200.0f, 100.0f);
		mcInfo.size = vec2(200.0f, 325.0f);
		mcInfo.flags = MAIN_COMPONENT_FLAGS_NO_RESIZE;
		CreateMainComponent(&mcInfo, &gGuiWindow);

		SubComponent shading{};
		shading.type = SUB_COMPONENT_TYPE_DROPDOWN;
		shading.dropDown.pLabel = "Shading";
		shading.dropDown.pData = &gCurrentShading;
		shading.dropDown.numNames = MAX_SHADING;
		shading.dropDown.pNames = gShadingNames;
		shading.dynamic = false;
		AddSubComponent(&gGuiWindow, &shading);

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
	}

	void Exit() override
	{
		WaitQueueIdle(&gRenderer, &gGraphicsQueue);

		DestroyMainComponent(&gGuiWindow);

		DestroyShape(&gVertices, &gIndices);

		DestroyUI(&gRenderer);
		DestroyDescriptorSet(&gDescriptorSetPerNone);
		DestroyDescriptorSet(&gDescriptorSetPerFrame);
		DestroyDescriptorSet(&gDescriptorSetPerDraw);

		DestroySampler(&gRenderer, &gSampler);
		DestroyTexture(&gRenderer, &gWoodColor);
		DestroyTexture(&gRenderer, &gWoodRoughness);
		DestroyTexture(&gRenderer, &gWoodNormal);
		DestroyTexture(&gRenderer, &gSkyboxTexture);

		for (uint32_t i = 0; i < gNumFrames; ++i)
		{
			DestroyBuffer(&gRenderer, &gShapesUniformBuffers[i]);
			DestroyBuffer(&gRenderer, &gCameraUniformBuffer[i]);
			DestroyBuffer(&gRenderer, &gLightSourceUniformBuffer[i]);
			DestroyBuffer(&gRenderer, &gSkyboxCameraBuffer[i]);
			DestroyBuffer(&gRenderer, &gSkyboxUniformBuffer[i]);
			DestroyBuffer(&gRenderer, &gPointLightUniformBuffer[i]);
			DestroyBuffer(&gRenderer, &gDirectionalLightUniformBuffer[i]);
			DestroyBuffer(&gRenderer, &gSpotlightUniformBuffer[i]);
			DestroySemaphore(&gRenderer, &gImageAvailableSemaphores[i]);
			DestroyCommandBuffer(&gRenderer, &gGraphicsCommandBuffers[i]);
		}

		DestroyPipeline(&gRenderer, &gSkyboxPipeline);
		DestroyPipeline(&gRenderer, &gPbrPipeline);
		DestroyPipeline(&gRenderer, &gPhongPipeline);
		DestroyPipeline(&gRenderer, &gShadowMapPipeline);
		DestroyPipeline(&gRenderer, &gLightSourcePipeline);

		DestroyRootSignature(&gRenderer, &gGraphicsRootSignature);

		DestroyBuffer(&gRenderer, &gIndexBuffer);
		DestroyBuffer(&gRenderer, &gVertexBuffer);

		DestroyShader(&gRenderer, &gSkyboxVS);
		DestroyShader(&gRenderer, &gObjectVS);
		DestroyShader(&gRenderer, &gLightSourceVS);
		DestroyShader(&gRenderer, &gShadowMapVS);

		DestroyShader(&gRenderer, &gSkyboxPS);
		DestroyShader(&gRenderer, &gPbrPS);
		DestroyShader(&gRenderer, &gPhongPS);
		DestroyShader(&gRenderer, &gShadowMapPS);
		DestroyShader(&gRenderer, &gLightSourcePS);

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
			LookAt(&gCamera, vec3(3.0f, 6.0f, -18.0f), vec3(3.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
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

		mat4 model[MAX_SHAPES];
		model[PLANE] = mat4::Scale(50.0f, 50.0f, 50.0f) * mat4::RotX(90.0f) * mat4::Translate(0.0f, 0.0f, 0.0f);
		model[BOX] = mat4::Scale(1.0f, 1.0f, 1.0f) * mat4::RotY(gAngle) * mat4::Translate(-10.0f, 0.5f, 0.0f);
		model[SPHERE] = mat4::Scale(1.0f, 1.0f, 1.0f) * mat4::RotY(gAngle) * mat4::Translate(-5.0f, 1.0f, 0.0f);
		model[HEMI_SPHERE] = mat4::Scale(1.0f, 1.0f, 1.0f) * mat4::RotY(gAngle) * mat4::Translate(0.0f, 1.0f, 0.0f);
		model[TORUS] = mat4::Scale(1.0f, 1.0f, 1.0f) * mat4::RotX(gAngle) * mat4::Translate(5.0f, 4.0f, 0.0f);
		model[CYLINDER] = mat4::Scale(1.0f, 5.0f, 1.0f) * mat4::RotX(gAngle) * mat4::Translate(10.0f, 4.0f, 0.0f);

		for (uint32_t i = 0; i < MAX_SHAPES; ++i)
		{
			gShapesData.objectModel[i] = Transpose(model[i]);
			gShapesData.objectInverseModel[i] = Inverse(model[i]);
		}

		vec4 lightColor = vec4(gLightColor.GetX(), gLightColor.GetY(), gLightColor.GetZ(), 1.0f);

		//Point light
		gPointLight.position = vec4(0.0f, 0.0f, -1.5f, 1.0f);
		gPointLight.color = lightColor;

		//Directional light
		gDirectionalLight.direction = vec4(0.0f, -5.0f, 10.0f, 0.0f);
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

			LookAt(&gPLCamera[4], position, position + vec3(0.0f, 0.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f)); //back
			LookAt(&gPLCamera[5], position, position + vec3(0.0f, 0.0f, -1.0f), vec3(0.0f, 1.0f, 0.0f)); //front

			for (uint32_t i = 0; i < 6; ++i)
			{
				gLightSourceDataPL[i].lightSourceModel = mat4::Scale(0.1f, 0.1f, 0.1f) *
					mat4::Translate(gPointLight.position.GetX(), gPointLight.position.GetY(), gPointLight.position.GetZ());

				UpdateViewMatrix(&gPLCamera[i]);
				UpdatePerspectiveProjectionMatrix(&gPLCamera[i]);

				gLightSourceDataPL[i].lightSourceView = Transpose(gPLCamera[i].viewMat);
				gLightSourceDataPL[i].lightSourceProjection = Transpose(gPLCamera[i].perspectiveProjMat);
			}

		}
		else if (gCurrentLightSource == DIRECTIONAL_LIGHT)
		{
			gLightSourceData.lightSourceModel = mat4::Scale(0.1f, 0.1f, 0.1f) *
				mat4::Translate(gDLCamera.position.GetX(), gDLCamera.position.GetY(), gDLCamera.position.GetZ());
		}
		else //SPOTLIGHT
		{
			gLightSourceData.lightSourceModel = mat4::Scale(0.1f, 0.1f, 0.1f) *
				mat4::Translate(gSpotlight.position.GetX(), gSpotlight.position.GetY(), gSpotlight.position.GetZ());
		}

		gLightSourceData.lightSourceView = Transpose(gDLCamera.viewMat);
		gLightSourceData.lightSourceProjection = Transpose(gDLCamera.orthographicProjMat);

		model[0] = mat4::Scale(1000.0f, 1000.0f, 1000.0f);
		gSkyBoxData.objectModel[0] = Transpose(model[0]);

		mat4 viewMat = gCamera.viewMat;
		viewMat.SetRow(3, 0.0f, 0.0f, 0.0f, 1.0f);
		gSkyboxCameraData.cameraView = Transpose(viewMat);
		gSkyboxCameraData.cameraProjection = Transpose(gCamera.perspectiveProjMat);
		gSkyboxCameraData.cameraPos = vec4(gCamera.position.GetX(), gCamera.position.GetY(), gCamera.position.GetZ(), 1.0f);

		gConstants.currentLightSource = gCurrentLightSource;

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

		MapMemory(&gRenderer, &gSkyboxUniformBuffer[gCurrentFrame], &data);
		memcpy(data, &gSkyBoxData, sizeof(ObjectData));
		UnmapMemory(&gRenderer, &gSkyboxUniformBuffer[gCurrentFrame]);

		MapMemory(&gRenderer, &gSkyboxCameraBuffer[gCurrentFrame], &data);
		memcpy(data, &gSkyboxCameraData, sizeof(CameraData));
		UnmapMemory(&gRenderer, &gSkyboxCameraBuffer[gCurrentFrame]);

		MapMemory(&gRenderer, &gShapesUniformBuffers[gCurrentFrame], &data);
		memcpy(data, &gShapesData, sizeof(ObjectData));
		UnmapMemory(&gRenderer, &gShapesUniformBuffers[gCurrentFrame]);

		MapMemory(&gRenderer, &gLightSourceUniformBuffer[gCurrentFrame], &data);
		memcpy(data, &gLightSourceData, sizeof(LightSourceData));
		UnmapMemory(&gRenderer, &gLightSourceUniformBuffer[gCurrentFrame]);

		uint32_t imageIndex = 0;
		AcquireNextImage(&gRenderer, &gSwapChain, &gImageAvailableSemaphores[gCurrentFrame], &imageIndex);

		RenderTarget* pRenderTarget = &gSwapChain.pRenderTargets[imageIndex];

		ResetCommandBuffer(&gRenderer, pCommandBuffer);

		BeginCommandBuffer(pCommandBuffer);

		BindVertexBuffer(pCommandBuffer, sizeof(Vertex), 0, 0, &gVertexBuffer);
		BindIndexBuffer(pCommandBuffer, 0, INDEX_TYPE_UINT32, &gIndexBuffer);

		//Draw to shadow map
		BarrierInfo barrierInfo[8]{};
		barrierInfo[0].type = BARRIER_TYPE_RENDER_TARGET;
		barrierInfo[0].pRenderTarget = &gShadowMap;
		barrierInfo[0].currentState = RESOURCE_STATE_ALL_SHADER_RESOURCE;
		barrierInfo[0].newState = RESOURCE_STATE_DEPTH_WRITE;

		for (uint32_t i = 0; i < 6; ++i)
		{
			barrierInfo[i + 1].type = BARRIER_TYPE_RENDER_TARGET;
			barrierInfo[i + 1].pRenderTarget = &gShadowMapPL[i];
			barrierInfo[i + 1].currentState = RESOURCE_STATE_ALL_SHADER_RESOURCE;
			barrierInfo[i + 1].newState = RESOURCE_STATE_DEPTH_WRITE;
		}
		ResourceBarrier(pCommandBuffer, 7, barrierInfo);

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

		//SHAPES
		BindPipeline(pCommandBuffer, &gShadowMapPipeline);
		BindDescriptorSet(pCommandBuffer, 0, 0, &gDescriptorSetPerNone);
		BindDescriptorSet(pCommandBuffer, gCurrentFrame, 1, &gDescriptorSetPerFrame);
		BindDescriptorSet(pCommandBuffer, gCurrentFrame, 2, &gDescriptorSetPerDraw);

		BindRenderTargetInfo renderTargetInfo{};
		renderTargetInfo.pRenderTarget = nullptr;
		renderTargetInfo.renderTargetLoadOp = LOAD_OP_CLEAR;
		renderTargetInfo.renderTargetStoreOp = STORE_OP_STORE;
		renderTargetInfo.pDepthTarget = &gShadowMap;
		renderTargetInfo.depthTargetLoadOp = LOAD_OP_CLEAR;
		renderTargetInfo.depthTargetStoreOp = STORE_OP_DONT_CARE;
		BindRenderTarget(pCommandBuffer, &renderTargetInfo);

		for (uint32_t i = 0; i < MAX_SHAPES; ++i)
		{
			gConstants.shape = i;
			BindRootConstants(pCommandBuffer, 2, sizeof(RootConstants), &gConstants, 0);
			DrawIndexedInstanced(pCommandBuffer, gIndexCounts[i], 1, gIndexOffsets[i], gVertexOffsets[i], 0);
		}

		//Point light shadow maps
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

			for (uint32_t j = 0; j < MAX_SHAPES; ++j)
			{
				gConstants.shape = j;
				BindRootConstants(pCommandBuffer, 2, sizeof(RootConstants), &gConstants, 0);
				DrawIndexedInstanced(pCommandBuffer, gIndexCounts[j], 1, gIndexOffsets[j], gVertexOffsets[j], 0);
			}
		}

		BindRenderTarget(pCommandBuffer, nullptr);

		//Draw to render target with shadow map
		barrierInfo[0].type = BARRIER_TYPE_RENDER_TARGET;
		barrierInfo[0].pRenderTarget = &gShadowMap;
		barrierInfo[0].currentState = RESOURCE_STATE_DEPTH_WRITE;
		barrierInfo[0].newState = RESOURCE_STATE_ALL_SHADER_RESOURCE;

		for (uint32_t i = 0; i < 6; ++i)
		{
			barrierInfo[i + 1].type = BARRIER_TYPE_RENDER_TARGET;
			barrierInfo[i + 1].pRenderTarget = &gShadowMapPL[i];
			barrierInfo[i + 1].currentState = RESOURCE_STATE_DEPTH_WRITE;
			barrierInfo[i + 1].newState = RESOURCE_STATE_ALL_SHADER_RESOURCE;
		}

		barrierInfo[7].type = BARRIER_TYPE_RENDER_TARGET;
		barrierInfo[7].pRenderTarget = pRenderTarget;
		barrierInfo[7].currentState = RESOURCE_STATE_PRESENT;
		barrierInfo[7].newState = RESOURCE_STATE_RENDER_TARGET;

		ResourceBarrier(pCommandBuffer, 8, barrierInfo);

		renderTargetInfo.pRenderTarget = pRenderTarget;
		renderTargetInfo.renderTargetLoadOp = LOAD_OP_CLEAR;
		renderTargetInfo.renderTargetStoreOp = STORE_OP_STORE;
		renderTargetInfo.pDepthTarget = &gDepthBuffer;
		renderTargetInfo.depthTargetLoadOp = LOAD_OP_CLEAR;
		renderTargetInfo.depthTargetStoreOp = STORE_OP_DONT_CARE;
		BindRenderTarget(pCommandBuffer, &renderTargetInfo);

		viewportInfo.x = 0.0f;
		viewportInfo.y = 0.0f;
		viewportInfo.width = pRenderTarget->info.width;
		viewportInfo.height = pRenderTarget->info.height;
		viewportInfo.minDepth = 0.0f;
		viewportInfo.maxDepth = 1.0f;
		SetViewport(pCommandBuffer, &viewportInfo);

		scissorInfo.x = 0.0f;
		scissorInfo.y = 0.0f;
		scissorInfo.width = pRenderTarget->info.width;
		scissorInfo.height = pRenderTarget->info.height;
		SetScissor(pCommandBuffer, &scissorInfo);

		//Draw Objects
		switch (gCurrentShading)
		{
		case PHONG:
			BindPipeline(pCommandBuffer, &gPhongPipeline);
			break;

		case PBR:
			BindPipeline(pCommandBuffer, &gPbrPipeline);
			break;
		}

		BindDescriptorSet(pCommandBuffer, 0, 0, &gDescriptorSetPerNone);
		BindDescriptorSet(pCommandBuffer, gCurrentFrame, 1, &gDescriptorSetPerFrame);
		BindDescriptorSet(pCommandBuffer, gCurrentFrame, 2, &gDescriptorSetPerDraw);

		for (uint32_t i = 0; i < MAX_SHAPES; ++i)
		{
			gConstants.shape = i;
			BindRootConstants(pCommandBuffer, 2, sizeof(RootConstants), &gConstants, 0);
			DrawIndexedInstanced(pCommandBuffer, gIndexCounts[i], 1, gIndexOffsets[i], gVertexOffsets[i], 0);
		}

		if (gShowLightSources)
		{
			//Draw Light Source
			BindPipeline(pCommandBuffer, &gLightSourcePipeline);
			BindDescriptorSet(pCommandBuffer, gCurrentFrame, 1, &gDescriptorSetPerFrame);
			DrawIndexedInstanced(pCommandBuffer, gIndexCounts[SPHERE], 1, gIndexOffsets[SPHERE], gVertexOffsets[SPHERE], 0);
		}

		//Draw Skybox
		BindPipeline(pCommandBuffer, &gSkyboxPipeline);
		BindDescriptorSet(pCommandBuffer, gCurrentFrame + 2, 1, &gDescriptorSetPerFrame);
		gConstants.shape = 0;
		BindRootConstants(pCommandBuffer, 2, sizeof(RootConstants), &gConstants, 0);
		DrawIndexedInstanced(pCommandBuffer, gIndexCounts[BOX], 1, gIndexOffsets[BOX], gVertexOffsets[BOX], 0);

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
	Lightning lightning;
	lightning.appName = "Lightning";
	return WindowsMain(&lightning);
};
