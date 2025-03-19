#include "../../../SecondEngine/SEApp.h"
#include "../../../SecondEngine/Renderer/SERenderer.h"
#include "../../../SecondEngine/Math/SEMath_Header.h"
#include "../../../SecondEngine/Renderer/SECamera.h"
#include "../../../SecondEngine/Time/SETimer.h"
#include "../../../SecondEngine/Shapes/SEShapes.h"
#include "../../../SecondEngine/UI/SEUI.h"
#include "../../Renderer/ShaderLibrary/lightSource.h"

Renderer gRenderer;

Queue gGraphicsQueue;

SwapChain gSwapChain;

RenderTarget gDepthBuffer;

RootSignature gGraphicsRootSignature;

Shader gPhongVS;
Shader gPhongPS;
Pipeline gPhongPipeline;

Shader gPbrVS;
Shader gPbrPS;
Pipeline gPbrPipeline;

Shader gLightSourceVS;
Shader gLightSourcePS;
Pipeline gLightSourcePipeline;

Shader gSkyboxVS;
Shader gSkyboxPS;
Pipeline gSkyboxPipeline;

const uint32_t gNumFrames = 2;
Semaphore gImageAvailableSemaphores[gNumFrames];
CommandBuffer gGraphicsCommandBuffers[gNumFrames];

Buffer gVertexBuffer;
Buffer gIndexBuffer;

Texture gBricksColor;
Texture gBricksAO;
Texture gBricksRoughness;
Texture gBricksNormal;
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
	vec4 cameraPos;
};

struct PerObjectUniformData
{
	mat4 model;
	mat4 transposeInverseModel;
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
	BOX,
	SQUARE_PYRAMID,
	TRIANGULAR_PYRAMID,
	SPHERE,
	HEMI_SPHERE,
	CYLINDER,
	CONE,
	TORUS,
	MAX_SHAPES
};

enum
{
	PHONG,
	PBR,
	MAX_SHADING
};

enum 
{
	OBJECT_COLOR,
	PEARL,
	COPPER,
	GOLD,
	SILVER,
	MAX_MATERIALS
};

enum
{
	MATERIAL,
	TEXTURE,
	MAX_MAPPINGS
};

enum
{
	POINT_LIGHT,
	DIRECTIONAL_LIGHT,
	SPOTLIGHT,
	ALL
};

PointLight gPointLight;
Buffer gPointLightUniformBuffer[gNumFrames];

DirectionalLight gDirectionalLight;
Buffer gDirectionalLightUniformBuffer[gNumFrames];

Spotlight gSpotlight;
Buffer gSpotlightUniformBuffer[gNumFrames];

PhongMaterial gPhongMaterialUniformData[MAX_MATERIALS];
Buffer gPhongMaterialUniformBuffer[gNumFrames];

PBRMaterial gPbrMaterialUniformData[MAX_MATERIALS];
Buffer gPbrMaterialUniformBuffer[gNumFrames];

PerFrameUniformData gPerFrameUniformData;
Buffer gPerFrameBuffer[gNumFrames];

PerObjectUniformData gShapesUniformData;
Buffer gShapesUniformBuffers[gNumFrames];

PerObjectUniformData gSkyBoxUniformData;
Buffer gSkyboxUniformBuffer[gNumFrames];

PerFrameUniformData gSkyboxPerFrameUniformData;
Buffer gSkyboxPerFrameBuffer[gNumFrames];

uint32_t gVertexOffsets[MAX_SHAPES];
uint32_t gIndexOffsets[MAX_SHAPES];
uint32_t gIndexCounts[MAX_SHAPES];

MainComponent gGuiWindow;

const char* gShapeNames[] = 
{ 
	"BOX",
	"SQUARED PYRAMID",
	"TRIANGULAR PYRAMID",
	"SPHERE",
	"HEMISPHERE",
	"CYLINDER",
	"CONE",
	"TORUS"
};
uint32_t gCurrentShape = 0;

const char* gMaterialNames[] =
{
	"OBJECT COLOR",
	"PEARL",
	"COPPER",
	"GOLD",
	"SILVER"
};
uint32_t gCurrentMaterial = 0;

const char* gMappingNames[] =
{
	"MATERIAL",
	"TEXTURE",
};
uint32_t gCurrentMapping = 0;

const char* gLightSourceNames[] =
{
	"POINT",
	"DIRECTIONAL",
	"SPOTLIGHT",
	"ALL"
};
uint32_t gCurrentLightSource = 0;

const char* gShadingNames[] = { "PHONG", "PBR"};
uint32_t gCurrentShading = 0;

bool gShowLightSources = false;
bool gRotate = false;

float gInnerCutoffAngle = 2.0f;
float gOuterCutoffAngle = 5.0f;

float gNormalScale = 3.0f;

struct RootConstants
{
	uint32_t currentMapping;
	uint32_t numPointLights;
	uint32_t numDirectionalLights;
	uint32_t numSpotlights;
	float normalScale;
};

RootConstants gConstants;

float gRoughness = 0.25f;
float gMetallic = 0.0f;
float gAO = 0.05f;

float gAngle = 0.0f;

vec3 gLightColor = vec3(1.0f, 1.0f, 1.0f);

SubComponent gMaterialSc{};
SubComponent gInnerCutoffSc{};
SubComponent gOuterCutoffSc{};
SubComponent gRoughnessSc{};
SubComponent gMetallicSc{};
SubComponent gAoSc{};
SubComponent gNormalScaleSC{};

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

		ShaderInfo shaderInfo{};

		shaderInfo.filename = "phong.vert";
		shaderInfo.type = SHADER_TYPE_VERTEX;
		CreateShader(&gRenderer, &shaderInfo, &gPhongVS);

		shaderInfo.filename = "phong.frag";
		shaderInfo.type = SHADER_TYPE_PIXEL;
		CreateShader(&gRenderer, &shaderInfo, &gPhongPS);

		shaderInfo.filename = "pbr.vert";
		shaderInfo.type = SHADER_TYPE_VERTEX;
		CreateShader(&gRenderer, &shaderInfo, &gPbrVS);

		shaderInfo.filename = "pbr.frag";
		shaderInfo.type = SHADER_TYPE_PIXEL;
		CreateShader(&gRenderer, &shaderInfo, &gPbrPS);

		shaderInfo.filename = "lightSource.vert";
		shaderInfo.type = SHADER_TYPE_VERTEX;
		CreateShader(&gRenderer, &shaderInfo, &gLightSourceVS);

		shaderInfo.filename = "lightSource.frag";
		shaderInfo.type = SHADER_TYPE_PIXEL;
		CreateShader(&gRenderer, &shaderInfo, &gLightSourcePS);

		shaderInfo.filename = "skybox.vert";
		shaderInfo.type = SHADER_TYPE_VERTEX;
		CreateShader(&gRenderer, &shaderInfo, &gSkyboxVS);

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

		RootParameterInfo rootParameterInfos[13]{};

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

		//Point light
		rootParameterInfos[2].binding = 2;
		rootParameterInfos[2].baseRegister = 2;
		rootParameterInfos[2].registerSpace = 0;
		rootParameterInfos[2].numDescriptors = 1;
		rootParameterInfos[2].stages = STAGE_VERTEX | STAGE_PIXEL;
		rootParameterInfos[2].type = DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		rootParameterInfos[2].updateFrequency = UPDATE_FREQUENCY_PER_FRAME;

		//Directional light
		rootParameterInfos[3].binding = 3;
		rootParameterInfos[3].baseRegister = 3;
		rootParameterInfos[3].registerSpace = 0;
		rootParameterInfos[3].numDescriptors = 1;
		rootParameterInfos[3].stages = STAGE_VERTEX | STAGE_PIXEL;
		rootParameterInfos[3].type = DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		rootParameterInfos[3].updateFrequency = UPDATE_FREQUENCY_PER_FRAME;

		//Spotlight
		rootParameterInfos[4].binding = 4;
		rootParameterInfos[4].baseRegister = 4;
		rootParameterInfos[4].registerSpace = 0;
		rootParameterInfos[4].numDescriptors = 1;
		rootParameterInfos[4].stages = STAGE_VERTEX | STAGE_PIXEL;
		rootParameterInfos[4].type = DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		rootParameterInfos[4].updateFrequency = UPDATE_FREQUENCY_PER_FRAME;

		//Phong material
		rootParameterInfos[5].binding = 5;
		rootParameterInfos[5].baseRegister = 5;
		rootParameterInfos[5].registerSpace = 0;
		rootParameterInfos[5].numDescriptors = 1;
		rootParameterInfos[5].stages = STAGE_VERTEX | STAGE_PIXEL;
		rootParameterInfos[5].type = DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		rootParameterInfos[5].updateFrequency = UPDATE_FREQUENCY_PER_FRAME;

		//PBR material
		rootParameterInfos[6].binding = 6;
		rootParameterInfos[6].baseRegister = 6;
		rootParameterInfos[6].registerSpace = 0;
		rootParameterInfos[6].numDescriptors = 1;
		rootParameterInfos[6].stages = STAGE_VERTEX | STAGE_PIXEL;
		rootParameterInfos[6].type = DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		rootParameterInfos[6].updateFrequency = UPDATE_FREQUENCY_PER_FRAME;

		//Bricks Color texture
		rootParameterInfos[7].binding = 0;
		rootParameterInfos[7].baseRegister = 0;
		rootParameterInfos[7].registerSpace = 0;
		rootParameterInfos[7].numDescriptors = 1;
		rootParameterInfos[7].stages = STAGE_PIXEL;
		rootParameterInfos[7].type = DESCRIPTOR_TYPE_TEXTURE;
		rootParameterInfos[7].updateFrequency = UPDATE_FREQUENCY_PER_NONE;

		//Bricks AO texture
		rootParameterInfos[8].binding = 1;
		rootParameterInfos[8].baseRegister = 1;
		rootParameterInfos[8].registerSpace = 0;
		rootParameterInfos[8].numDescriptors = 1;
		rootParameterInfos[8].stages = STAGE_PIXEL;
		rootParameterInfos[8].type = DESCRIPTOR_TYPE_TEXTURE;
		rootParameterInfos[8].updateFrequency = UPDATE_FREQUENCY_PER_NONE;

		//Bricks Roughness texture
		rootParameterInfos[9].binding = 2;
		rootParameterInfos[9].baseRegister = 2;
		rootParameterInfos[9].registerSpace = 0;
		rootParameterInfos[9].numDescriptors = 1;
		rootParameterInfos[9].stages = STAGE_PIXEL;
		rootParameterInfos[9].type = DESCRIPTOR_TYPE_TEXTURE;
		rootParameterInfos[9].updateFrequency = UPDATE_FREQUENCY_PER_NONE;

		//Bricks Normal texture
		rootParameterInfos[10].binding = 3;
		rootParameterInfos[10].baseRegister = 3;
		rootParameterInfos[10].registerSpace = 0;
		rootParameterInfos[10].numDescriptors = 1;
		rootParameterInfos[10].stages = STAGE_PIXEL;
		rootParameterInfos[10].type = DESCRIPTOR_TYPE_TEXTURE;
		rootParameterInfos[10].updateFrequency = UPDATE_FREQUENCY_PER_NONE;

		//Cubemap
		rootParameterInfos[11].binding = 4;
		rootParameterInfos[11].baseRegister = 4;
		rootParameterInfos[11].registerSpace = 0;
		rootParameterInfos[11].numDescriptors = 1;
		rootParameterInfos[11].stages = STAGE_PIXEL;
		rootParameterInfos[11].type = DESCRIPTOR_TYPE_TEXTURE;
		rootParameterInfos[11].updateFrequency = UPDATE_FREQUENCY_PER_NONE;

		//Sampler
		rootParameterInfos[12].binding = 5;
		rootParameterInfos[12].baseRegister = 0;
		rootParameterInfos[12].registerSpace = 0;
		rootParameterInfos[12].numDescriptors = 1;
		rootParameterInfos[12].stages = STAGE_PIXEL;
		rootParameterInfos[12].type = DESCRIPTOR_TYPE_SAMPLER;
		rootParameterInfos[12].updateFrequency = UPDATE_FREQUENCY_PER_NONE;

		RootConstantsInfo rootConstantInfo{};
		rootConstantInfo.numValues = 5;
		rootConstantInfo.baseRegister = 7;
		rootConstantInfo.registerSpace = 0;
		rootConstantInfo.stride = sizeof(RootConstants);
		rootConstantInfo.stages = STAGE_VERTEX | STAGE_PIXEL;

		RootSignatureInfo graphicsRootSignatureInfo{};
		graphicsRootSignatureInfo.pRootParameterInfos = rootParameterInfos;
		graphicsRootSignatureInfo.numRootParameterInfos = 13;
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
		graphicsPipelineInfo.pVertexShader = &gPhongVS;
		graphicsPipelineInfo.pPixelShader = &gPhongPS;
		graphicsPipelineInfo.renderTargetFormat = gSwapChain.pRenderTargets[0].info.format;
		graphicsPipelineInfo.depthInfo.depthTestEnable = true;
		graphicsPipelineInfo.depthInfo.depthWriteEnable = true;
		graphicsPipelineInfo.depthInfo.depthFunction = DEPTH_FUNCTION_LESS;
		graphicsPipelineInfo.depthFormat = gDepthBuffer.info.format;
		graphicsPipelineInfo.pVertexInputInfo = &vertexInputInfo;
		graphicsPipelineInfo.pRootSignature = &gGraphicsRootSignature;
		CreatePipeline(&gRenderer, &graphicsPipelineInfo, &gPhongPipeline);

		graphicsPipelineInfo.pVertexShader = &gPbrVS;
		graphicsPipelineInfo.pPixelShader = &gPbrPS;
		CreatePipeline(&gRenderer, &graphicsPipelineInfo, &gPbrPipeline);

		graphicsPipelineInfo.pVertexShader = &gLightSourceVS;
		graphicsPipelineInfo.pPixelShader = &gLightSourcePS;
		CreatePipeline(&gRenderer, &graphicsPipelineInfo, &gLightSourcePipeline);

		graphicsPipelineInfo.rasInfo.cullMode = CULL_MODE_NONE;
		graphicsPipelineInfo.pVertexShader = &gSkyboxVS;
		graphicsPipelineInfo.pPixelShader = &gSkyboxPS;
		graphicsPipelineInfo.depthInfo.depthFunction = DEPTH_FUNCTION_LESS_OR_EQUAL;
		CreatePipeline(&gRenderer, &graphicsPipelineInfo, &gSkyboxPipeline);

		for (uint32_t i = 0; i < gNumFrames; ++i)
		{
			CreateCommandBuffer(&gRenderer, QUEUE_TYPE_GRAPHICS, &gGraphicsCommandBuffers[i]);
			CreateSemaphore(&gRenderer, &gImageAvailableSemaphores[i]);
		}

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

		gVertexOffsets[CYLINDER] = arrlenu(gVertices);
		gIndexOffsets[CYLINDER] = arrlenu(gIndices);
		CreateCylinder(&gVertices, &gIndices, &gIndexCounts[CYLINDER], false, false);

		gVertexOffsets[CONE] = arrlenu(gVertices);
		gIndexOffsets[CONE] = arrlenu(gIndices);
		CreateCone(&gVertices, &gIndices, &gIndexCounts[CONE], false);

		gVertexOffsets[TORUS] = arrlenu(gVertices);
		gIndexOffsets[TORUS] = arrlenu(gIndices);
		CreateTorus(&gVertices, &gIndices, &gIndexCounts[TORUS], 1.0f, 0.5f);

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
			CreateBuffer(&gRenderer, &ubInfo, &gSkyboxUniformBuffer[i]);

			ubInfo.size = sizeof(PointLight);
			CreateBuffer(&gRenderer, &ubInfo, &gPointLightUniformBuffer[i]);

			ubInfo.size = sizeof(DirectionalLight);
			CreateBuffer(&gRenderer, &ubInfo, &gDirectionalLightUniformBuffer[i]);

			ubInfo.size = sizeof(Spotlight);
			CreateBuffer(&gRenderer, &ubInfo, &gSpotlightUniformBuffer[i]);

			ubInfo.size = sizeof(PhongMaterial);
			CreateBuffer(&gRenderer, &ubInfo, &gPhongMaterialUniformBuffer[i]);

			ubInfo.size = sizeof(PBRMaterial);
			CreateBuffer(&gRenderer, &ubInfo, &gPbrMaterialUniformBuffer[i]);
		}

		TextureInfo texInfo{};
		texInfo.filename = "Textures/bricksColor.dds";
		CreateTexture(&gRenderer, &texInfo, &gBricksColor);

		texInfo.filename = "Textures/bricksAO.dds";
		CreateTexture(&gRenderer, &texInfo, &gBricksAO);

		texInfo.filename = "Textures/bricksRoughness.dds";
		CreateTexture(&gRenderer, &texInfo, &gBricksRoughness);

		texInfo.filename = "Textures/bricksNormal.dds";
		CreateTexture(&gRenderer, &texInfo, &gBricksNormal);

		texInfo.filename = "Textures/yokohama.dds";
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
		uniformSetInfo.numSets = gNumFrames + 2;
		uniformSetInfo.updateFrequency = UPDATE_FREQUENCY_PER_FRAME;
		CreateDescriptorSet(&gRenderer, &uniformSetInfo, &gDescriptorSetPerFrame);

		DescriptorSetInfo texturesSetInfo{};
		texturesSetInfo.pRootSignature = &gGraphicsRootSignature;
		texturesSetInfo.numSets = 2;
		texturesSetInfo.updateFrequency = UPDATE_FREQUENCY_PER_NONE;
		CreateDescriptorSet(&gRenderer, &texturesSetInfo, &gDescriptorSetPerNone);

		for (uint32_t i = 0; i < gNumFrames; ++i)
		{
			UpdateDescriptorSetInfo updatePerFrame[7]{};
			updatePerFrame[0].binding = 0;
			updatePerFrame[0].type = UPDATE_TYPE_UNIFORM_BUFFER;
			updatePerFrame[0].pBuffer = &gPerFrameBuffer[i];

			updatePerFrame[1].binding = 1;
			updatePerFrame[1].type = UPDATE_TYPE_UNIFORM_BUFFER;
			updatePerFrame[1].pBuffer = &gShapesUniformBuffers[i];

			updatePerFrame[2].binding = 2;
			updatePerFrame[2].type = UPDATE_TYPE_UNIFORM_BUFFER;
			updatePerFrame[2].pBuffer = &gPointLightUniformBuffer[i];

			updatePerFrame[3].binding = 3;
			updatePerFrame[3].type = UPDATE_TYPE_UNIFORM_BUFFER;
			updatePerFrame[3].pBuffer = &gDirectionalLightUniformBuffer[i];

			updatePerFrame[4].binding = 4;
			updatePerFrame[4].type = UPDATE_TYPE_UNIFORM_BUFFER;
			updatePerFrame[4].pBuffer = &gSpotlightUniformBuffer[i];

			updatePerFrame[5].binding = 5;
			updatePerFrame[5].type = UPDATE_TYPE_UNIFORM_BUFFER;
			updatePerFrame[5].pBuffer = &gPhongMaterialUniformBuffer[i];

			updatePerFrame[6].binding = 6;
			updatePerFrame[6].type = UPDATE_TYPE_UNIFORM_BUFFER;
			updatePerFrame[6].pBuffer = &gPbrMaterialUniformBuffer[i];

			UpdateDescriptorSet(&gRenderer, &gDescriptorSetPerFrame, i, 7, updatePerFrame);

			updatePerFrame[0].binding = 0;
			updatePerFrame[0].type = UPDATE_TYPE_UNIFORM_BUFFER;
			updatePerFrame[0].pBuffer = &gSkyboxPerFrameBuffer[i];

			updatePerFrame[1].binding = 1;
			updatePerFrame[1].type = UPDATE_TYPE_UNIFORM_BUFFER;
			updatePerFrame[1].pBuffer = &gSkyboxUniformBuffer[i];

			UpdateDescriptorSet(&gRenderer, &gDescriptorSetPerFrame, i + 2, 7, updatePerFrame);
		}

		UpdateDescriptorSetInfo updateImageSetInfo[6]{};
		updateImageSetInfo[0].binding = 0;
		updateImageSetInfo[0].type = UPDATE_TYPE_TEXTURE;
		updateImageSetInfo[0].pTexture = &gBricksColor;

		updateImageSetInfo[1].binding = 1;
		updateImageSetInfo[1].type = UPDATE_TYPE_TEXTURE;
		updateImageSetInfo[1].pTexture = &gBricksAO;

		updateImageSetInfo[2].binding = 2;
		updateImageSetInfo[2].type = UPDATE_TYPE_TEXTURE;
		updateImageSetInfo[2].pTexture = &gBricksRoughness;

		updateImageSetInfo[3].binding = 3;
		updateImageSetInfo[3].type = UPDATE_TYPE_TEXTURE;
		updateImageSetInfo[3].pTexture = &gBricksNormal;

		updateImageSetInfo[4].binding = 4;
		updateImageSetInfo[4].type = UPDATE_TYPE_TEXTURE;
		updateImageSetInfo[4].pTexture = &gSkyboxTexture;

		updateImageSetInfo[5].binding = 5;
		updateImageSetInfo[5].type = UPDATE_TYPE_SAMPLER;
		updateImageSetInfo[5].pSampler = &gSampler;
		UpdateDescriptorSet(&gRenderer, &gDescriptorSetPerNone, 0, 6, updateImageSetInfo);

		updateImageSetInfo[1].pTexture = &gSkyboxTexture;
		UpdateDescriptorSet(&gRenderer, &gDescriptorSetPerNone, 1, 6, updateImageSetInfo);

		gPhongMaterialUniformData[OBJECT_COLOR].diffuse = vec4(0.0f, 0.5f, 0.5f, 1.0f);
		gPhongMaterialUniformData[OBJECT_COLOR].specular = vec4(0.5f, 0.5f, 0.5f, 1.0f);
		gPhongMaterialUniformData[OBJECT_COLOR].shininess = 512.0f;

		//Values from http://devernay.free.fr/cours/opengl/materials.html
		gPhongMaterialUniformData[PEARL].diffuse = vec4(1.0f, 0.829f, 0.829f, 1.0f);
		gPhongMaterialUniformData[PEARL].specular = vec4(0.296648f, 0.296648f, 0.296648f, 1.0f);
		gPhongMaterialUniformData[PEARL].shininess = 0.088f * 128.0f;

		gPhongMaterialUniformData[COPPER].diffuse = vec4(0.7038f, 0.27048f, 0.0828f, 1.0f);
		gPhongMaterialUniformData[COPPER].specular = vec4(0.256777f, 0.137622f, 0.086014f, 1.0f);
		gPhongMaterialUniformData[COPPER].shininess = 0.1f * 128.0f;

		gPhongMaterialUniformData[GOLD].diffuse = vec4(0.75164f, 0.60648f, 0.22648f, 1.0f);
		gPhongMaterialUniformData[GOLD].specular = vec4(0.628281f, 0.555802f, 0.366065f, 1.0f);
		gPhongMaterialUniformData[GOLD].shininess = 0.4f * 128.0f;

		gPhongMaterialUniformData[SILVER].diffuse = vec4(0.50754f, 0.50754f, 0.50754f, 1.0f);
		gPhongMaterialUniformData[SILVER].specular = vec4(0.508273f, 0.508273f, 0.508273f, 1.0f);
		gPhongMaterialUniformData[SILVER].shininess = 0.4f * 128.0f;

		gPbrMaterialUniformData[OBJECT_COLOR].albedo = vec4(0.0f, 0.5f, 0.5f, 1.0f);

		//Values from https://physicallybased.info/
		gPbrMaterialUniformData[PEARL].albedo = vec4(1.0f, 0.829f, 0.829f, 1.0f);

		gPbrMaterialUniformData[COPPER].albedo = vec4(0.7038f, 0.27048f, 0.0828f, 1.0f);
		
		gPbrMaterialUniformData[GOLD].albedo = vec4(0.75164f, 0.60648f, 0.22648f, 1.0f);

		gPbrMaterialUniformData[SILVER].albedo = vec4(0.50754f, 0.50754f, 0.50754f, 1.0f);

		LookAt(&gCamera, vec3(0.0f, 2.0f, -8.0f), vec3(0.0f, 0.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f));

		gCamera.vFov = 45.0f;
		gCamera.nearP = 1.0f;
		gCamera.farP = 100.0f;

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
		mcInfo.position = vec2(GetWidth(pWindow) - 275.0f, 100.0f);
		mcInfo.size = vec2(275.0f, 600.0f);
		mcInfo.flags = MAIN_COMPONENT_FLAGS_NO_RESIZE;
		CreateMainComponent(&mcInfo, &gGuiWindow);

		SubComponent shapes{};
		shapes.type = SUB_COMPONENT_TYPE_DROPDOWN;
		shapes.dropDown.pLabel = "Shape";
		shapes.dropDown.pData = &gCurrentShape;
		shapes.dropDown.numNames = MAX_SHAPES;
		shapes.dropDown.pNames = gShapeNames;
		shapes.dynamic = false;
		AddSubComponent(&gGuiWindow, &shapes);

		SubComponent shading{};
		shading.type = SUB_COMPONENT_TYPE_DROPDOWN;
		shading.dropDown.pLabel = "Shading";
		shading.dropDown.pData = &gCurrentShading;
		shading.dropDown.numNames = MAX_SHADING;
		shading.dropDown.pNames = gShadingNames;
		shading.dynamic = false;
		AddSubComponent(&gGuiWindow, &shading);

		SubComponent mapping{};
		mapping.type = SUB_COMPONENT_TYPE_DROPDOWN;
		mapping.dropDown.pLabel = "Mapping";
		mapping.dropDown.pData = &gCurrentMapping;
		mapping.dropDown.numNames = MAX_MAPPINGS;
		mapping.dropDown.pNames = gMappingNames;
		mapping.dynamic = false;
		AddSubComponent(&gGuiWindow, &mapping);

		gMaterialSc.type = SUB_COMPONENT_TYPE_DROPDOWN;
		gMaterialSc.dropDown.pLabel = "Material";
		gMaterialSc.dropDown.pData = &gCurrentMaterial;
		gMaterialSc.dropDown.numNames = MAX_MATERIALS;
		gMaterialSc.dropDown.pNames = gMaterialNames;
		gMaterialSc.dynamic = true;
		gMaterialSc.show = false;
		AddSubComponent(&gGuiWindow, &gMaterialSc);

		SubComponent lightSource{};
		lightSource.type = SUB_COMPONENT_TYPE_DROPDOWN;
		lightSource.dropDown.pLabel = "Light Source";
		lightSource.dropDown.pData = &gCurrentLightSource;
		lightSource.dropDown.numNames = 4;
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

		gRoughnessSc.type = SUB_COMPONENT_TYPE_SLIDER_FLOAT;
		gRoughnessSc.sliderFloat.pLabel = "Roughness";
		gRoughnessSc.sliderFloat.min = 0.0f;
		gRoughnessSc.sliderFloat.max = 1.0f;
		gRoughnessSc.sliderFloat.stepRate = 0.05f;
		gRoughnessSc.sliderFloat.pData = &gRoughness;
		gRoughnessSc.sliderFloat.format = "%.3f";
		gRoughnessSc.dynamic = true;
		gRoughnessSc.show = false;
		AddSubComponent(&gGuiWindow, &gRoughnessSc);

		gMetallicSc.type = SUB_COMPONENT_TYPE_SLIDER_FLOAT;
		gMetallicSc.sliderFloat.pLabel = "Metallic";
		gMetallicSc.sliderFloat.min = 0.0f;
		gMetallicSc.sliderFloat.max = 1.0f;
		gMetallicSc.sliderFloat.stepRate = 0.05f;
		gMetallicSc.sliderFloat.pData = &gMetallic;
		gMetallicSc.sliderFloat.format = "%.3f";
		gMetallicSc.dynamic = true;
		gMetallicSc.show = false;
		AddSubComponent(&gGuiWindow, &gMetallicSc);

		gAoSc.type = SUB_COMPONENT_TYPE_SLIDER_FLOAT;
		gAoSc.sliderFloat.pLabel = "AO";
		gAoSc.sliderFloat.min = 0.0f;
		gAoSc.sliderFloat.max = 1.0f;
		gAoSc.sliderFloat.stepRate = 0.025f;
		gAoSc.sliderFloat.pData = &gAO;
		gAoSc.sliderFloat.format = "%.3f";
		gAoSc.dynamic = true;
		gAoSc.show = false;
		AddSubComponent(&gGuiWindow, &gAoSc);

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

		gNormalScaleSC.type = SUB_COMPONENT_TYPE_SLIDER_FLOAT;
		gNormalScaleSC.sliderFloat.pLabel = "Normal Scale";
		gNormalScaleSC.sliderFloat.min = 0.0f;
		gNormalScaleSC.sliderFloat.max = 10.0f;
		gNormalScaleSC.sliderFloat.stepRate = 0.1f;
		gNormalScaleSC.sliderFloat.pData = &gNormalScale;
		gNormalScaleSC.sliderFloat.format = "%.3f";
		gNormalScaleSC.dynamic = true;
		gNormalScaleSC.show = false;
		AddSubComponent(&gGuiWindow, &gNormalScaleSC);
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
		DestroyTexture(&gRenderer, &gBricksColor);
		DestroyTexture(&gRenderer, &gBricksAO);
		DestroyTexture(&gRenderer, &gBricksRoughness);
		DestroyTexture(&gRenderer, &gBricksNormal);
		DestroyTexture(&gRenderer, &gSkyboxTexture);

		for (uint32_t i = 0; i < gNumFrames; ++i)
		{
			DestroyBuffer(&gRenderer, &gPerFrameBuffer[i]);
			DestroyBuffer(&gRenderer, &gSkyboxPerFrameBuffer[i]);
			DestroyBuffer(&gRenderer, &gSkyboxUniformBuffer[i]);
			DestroyBuffer(&gRenderer, &gShapesUniformBuffers[i]);
			DestroyBuffer(&gRenderer, &gPointLightUniformBuffer[i]);
			DestroyBuffer(&gRenderer, &gDirectionalLightUniformBuffer[i]);
			DestroyBuffer(&gRenderer, &gSpotlightUniformBuffer[i]);
			DestroyBuffer(&gRenderer, &gPhongMaterialUniformBuffer[i]);
			DestroyBuffer(&gRenderer, &gPbrMaterialUniformBuffer[i]);
			DestroySemaphore(&gRenderer, &gImageAvailableSemaphores[i]);
			DestroyCommandBuffer(&gRenderer, &gGraphicsCommandBuffers[i]);
		}

		DestroyPipeline(&gRenderer, &gSkyboxPipeline);
		DestroyPipeline(&gRenderer, &gPbrPipeline);
		DestroyPipeline(&gRenderer, &gPhongPipeline);
		DestroyPipeline(&gRenderer, &gLightSourcePipeline);

		DestroyRootSignature(&gRenderer, &gGraphicsRootSignature);

		DestroyBuffer(&gRenderer, &gIndexBuffer);
		DestroyBuffer(&gRenderer, &gVertexBuffer);

		DestroyShader(&gRenderer, &gSkyboxVS);
		DestroyShader(&gRenderer, &gSkyboxPS);
		DestroyShader(&gRenderer, &gPbrVS);
		DestroyShader(&gRenderer, &gPbrPS);
		DestroyShader(&gRenderer, &gPhongVS);
		DestroyShader(&gRenderer, &gPhongPS);
		DestroyShader(&gRenderer, &gLightSourceVS);
		DestroyShader(&gRenderer, &gLightSourcePS);

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
			LookAt(&gCamera, vec3(0.0f, 2.0f, -8.0f), vec3(0.0f, 0.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f));
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
		gPerFrameUniformData.cameraPos = vec4(gCamera.position.GetX(), gCamera.position.GetY(), gCamera.position.GetZ(), 1.0f);
		gPerFrameUniformData.projection = Transpose(gCamera.perspectiveProjMat);

		if (gRotate)
		{
			gAngle += 45.0f * deltaTime;
			if (gAngle >= 360.0f)
				gAngle = 0.0f;
		}

		mat4 model = mat4::Scale(1.0f, 1.0f, 1.0f) * mat4::RotY(gAngle) * mat4::Translate(0.0f, 0.0f, 0.0f);
		if (gCurrentShape == TORUS)
		{
			model.SetRow(3, 0.0f, 0.0f, 1.0f, 1.0f);
		}
		gShapesUniformData.model = Transpose(model);
		gShapesUniformData.transposeInverseModel = Inverse(model);

		vec4 lightColor = vec4(gLightColor.GetX(), gLightColor.GetY(), gLightColor.GetZ(), 1.0f);

		//Point light
		gPointLight.position = vec4(0.0f, 0.0f, -1.5f, 1.0f);
		gPointLight.color = lightColor;
		model = mat4::Scale(0.1f, 0.1f, 0.1f) * 
			mat4::Translate(gPointLight.position.GetX(), gPointLight.position.GetY(), gPointLight.position.GetZ());

		//Directional light
		gDirectionalLight.direction = vec4(0.0f, 0.0f, -1.0f, 0.0f);
		gDirectionalLight.color = lightColor;

		//Spotlight
		gSpotlight.position = vec4(-2.0f, 0.0f, 0.0f, 1.0f);
		if (gCurrentShape == TORUS)
		{
			gSpotlight.position = vec4(-2.0f, 0.0f, 1.0f, 1.0f);
		}
		gSpotlight.direction = vec4(1.0f, 0.0f, 0.0f, 0.0f);
		gSpotlight.color = lightColor;
		gSpotlight.innerCutoff = cos(gInnerCutoffAngle * PI / 180.0f);
		gSpotlight.outerCutoff = cos(gOuterCutoffAngle * PI / 180.0f);
		model = mat4::Scale(0.1f, 0.1f, 0.1f) * 
			mat4::Translate(gSpotlight.position.GetX(), gSpotlight.position.GetY(), gSpotlight.position.GetZ());

		for (uint32_t i = 0; i < MAX_MATERIALS; ++i)
		{
			gPbrMaterialUniformData[i].roughness = gRoughness;
			gPbrMaterialUniformData[i].metallic = gMetallic;
			gPbrMaterialUniformData[i].ao = gAO;

			gPhongMaterialUniformData[i].ambientIntensity = gAO;
		}

		gConstants.currentMapping = gCurrentMapping;
		gConstants.normalScale = gNormalScale;

		model = mat4::Scale(1000.0f, 1000.0f, 1000.0f);
		gSkyBoxUniformData.model = Transpose(model);

		mat4 viewMat = gCamera.viewMat;
		viewMat.SetRow(3, 0.0f, 0.0f, 0.0f, 1.0f);
		gSkyboxPerFrameUniformData.view = Transpose(viewMat);
		gSkyboxPerFrameUniformData.projection = Transpose(gCamera.perspectiveProjMat);

		if (gCurrentShading == PBR)
		{
			if (gCurrentMapping == TEXTURE)
			{
				gRoughnessSc.show = false;
				gMetallicSc.show = false;
			}
			else
			{
				gRoughnessSc.show = true;
				gMetallicSc.show = true;
			}
		}
		else
		{
			gRoughnessSc.show = false;
			gMetallicSc.show = false;
		}

		if (gCurrentLightSource == SPOTLIGHT || gCurrentLightSource == ALL)
		{
			gInnerCutoffSc.show = true;
			gOuterCutoffSc.show = true;

		}
		else
		{
			gInnerCutoffSc.show = false;
			gOuterCutoffSc.show = false;
		}

		if (gCurrentMapping == MATERIAL)
		{
			gMaterialSc.show = true;
			gNormalScaleSC.show = false;
			gAoSc.show = true;
		}
		else
		{
			gMaterialSc.show = false;
			gNormalScaleSC.show = true;
			gAoSc.show = false;
		}

		if (gCurrentLightSource == POINT_LIGHT)
		{
			gConstants.numPointLights = 1;
			gConstants.numDirectionalLights = 0;
			gConstants.numSpotlights = 0;
		}
		else if (gCurrentLightSource == DIRECTIONAL_LIGHT)
		{
			gConstants.numPointLights = 0;
			gConstants.numDirectionalLights = 1;
			gConstants.numSpotlights = 0;
		}
		else if (gCurrentLightSource == SPOTLIGHT)
		{
			gConstants.numPointLights = 0;
			gConstants.numDirectionalLights = 0;
			gConstants.numSpotlights = 1;
		}
		else //ALL
		{
			gConstants.numPointLights = 1;
			gConstants.numDirectionalLights = 1;
			gConstants.numSpotlights = 1;
		}
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

		MapMemory(&gRenderer, &gPointLightUniformBuffer[gCurrentFrame], &data);
		memcpy(data, &gPointLight, sizeof(PointLight));
		UnmapMemory(&gRenderer, &gPointLightUniformBuffer[gCurrentFrame]);

		MapMemory(&gRenderer, &gDirectionalLightUniformBuffer[gCurrentFrame], &data);
		memcpy(data, &gDirectionalLight, sizeof(DirectionalLight));
		UnmapMemory(&gRenderer, &gDirectionalLightUniformBuffer[gCurrentFrame]);

		MapMemory(&gRenderer, &gSpotlightUniformBuffer[gCurrentFrame], &data);
		memcpy(data, &gSpotlight, sizeof(Spotlight));
		UnmapMemory(&gRenderer, &gSpotlightUniformBuffer[gCurrentFrame]);

		MapMemory(&gRenderer, &gPhongMaterialUniformBuffer[gCurrentFrame], &data);
		memcpy(data, &gPhongMaterialUniformData[gCurrentMaterial], sizeof(PhongMaterial));
		UnmapMemory(&gRenderer, &gPhongMaterialUniformBuffer[gCurrentFrame]);

		MapMemory(&gRenderer, &gPbrMaterialUniformBuffer[gCurrentFrame], &data);
		memcpy(data, &gPbrMaterialUniformData[gCurrentMaterial], sizeof(PBRMaterial));
		UnmapMemory(&gRenderer, &gPbrMaterialUniformBuffer[gCurrentFrame]);

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

		//Draw Shape
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
		BindRootConstants(pCommandBuffer, 5, sizeof(RootConstants), &gConstants, 0);
		DrawIndexedInstanced(pCommandBuffer, gIndexCounts[gCurrentShape], 1, gIndexOffsets[gCurrentShape], gVertexOffsets[gCurrentShape], 0);

		if (gShowLightSources)
		{
			//Draw Light Source
			BindPipeline(pCommandBuffer, &gLightSourcePipeline);
			DrawIndexedInstanced(pCommandBuffer, gIndexCounts[SPHERE], 1, gIndexOffsets[SPHERE], gVertexOffsets[SPHERE], 0);
			BindPipeline(pCommandBuffer, &gLightSourcePipeline);
		}

		//Draw Skybox
		BindPipeline(pCommandBuffer, &gSkyboxPipeline);
		BindDescriptorSet(pCommandBuffer, 1, 0, &gDescriptorSetPerNone);
		BindDescriptorSet(pCommandBuffer, gCurrentFrame + 2, 1, &gDescriptorSetPerFrame);
		DrawIndexedInstanced(pCommandBuffer, gIndexCounts[BOX], 1,
			gIndexOffsets[BOX], gVertexOffsets[BOX], 0);

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
	Lightning lightning;
	lightning.appName = "Lightning";
	return WindowsMain(&lightning);
};
