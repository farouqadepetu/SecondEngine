#include "Renderer\SEWindow.h"
#include "SEApp.h"
#include "Renderer\Vulkan\SEVulkan.h"
#include "Math\SEMath_Header.h"
#include "Renderer\SECamera.h"
#include "Time\SETimer.h"

extern void (*gFuncOnKeyReleased)(int);
extern void (*gOnResize)();

Window gMainWindow;

Vulkan gVulkan;

VulkanQueue gGraphicsQueue;

VulkanSwapChain gSwapChain;

VulkanRenderAttachment gDepthBuffer;

VulkanShader gVertexShader;
VulkanShader gPixelShader;
VulkanDescriptorSetLayout gGraphicsLayout;
VulkanPipeline gGraphicsPipeline;

VulkanShader gComputeShader;
VulkanDescriptorSetLayout gComputeLayout;
VulkanPipeline gComputePipeline;

VulkanCommandPool gCommandPool;

const uint32_t gNumFrames = 2;
VulkanCommandBuffer gCommandBuffers[gNumFrames];
VulkanSemaphore gImageAvailableSemaphores[gNumFrames];
VulkanSemaphore gRenderFinishedSemaphores[gNumFrames];
VulkanFence gFences[gNumFrames];

VulkanBuffer gVertexBuffer;
VulkanBuffer gIndexBuffer;

VulkanBuffer gUniformBuffer[gNumFrames];

VulkanTexture gStatueTexture;
VulkanSampler gStatueSampler;

uint32_t gCurrentFrame = 0;

VulkanDescriptorSet gGraphicsDescriptorSet;

VulkanDescriptorSet gComputeDescriptorSet;

Camera gCamera;
Timer gTimer;

struct Vertex
{
	vec4 position;
	vec4 color;
	vec2 texCoords;
};

const Vertex triangleVertices[3] =
{ 
	{{vec4(0.0f, 0.5f, 0.0f, 1.0f)}, {vec4(1.0f, 0.0f, 0.0f, 1.0f)}},
	{{vec4(0.5f, 0.0f, 0.0f, 1.0f)}, {vec4(0.0f, 1.0f, 0.0f, 1.0f)}},
	{{vec4(-0.5f, 0.0f, 0.0f, 1.0f)}, {vec4(0.0f, 0.0f, 1.0f, 1.0f)}}
};

const uint32_t triangleIndices[3] = { 0, 1, 2 };


const Vertex quadVertices[4] =
{
	{{vec4(0.5f, 0.5f, 0.0f, 1.0f)}, {vec4(1.0f, 0.0f, 0.0f, 1.0f)}, {vec2(1.0f, 0.0f)} },
	{{vec4(0.5f, -0.5f, 0.0f, 1.0f)}, {vec4(0.0f, 1.0f, 0.0f, 1.0f)}, {vec2(1.0f, 1.0f)} },
	{{vec4(-0.5f, 0.5f, 0.0f, 1.0f)}, {vec4(0.0f, 0.0f, 1.0f, 1.0f)}, {vec2(0.0f, 0.0f)} },
	{{vec4(-0.5f, -0.5f, 0.0f, 1.0f)}, {vec4(1.0f, 1.0f, 1.0f, 1.0f)}, {vec2(0.0f, 1.0f)} }
};

const uint32_t quadIndices[6] = { 0, 1, 2, 3, 2, 1 };

struct UniformData
{
	mat4 model[12];
	mat4 view[12];
	mat4 projection[12];
};

UniformData gUniformData;

void OnResize()
{
	VulkanOnResize(&gVulkan, &gSwapChain,&gDepthBuffer);
}

mat4 gCubeRotation;
float gAngle = 0.0f;
void OnKeyReleased(int vKey)
{
	/*switch (vKey)
	{
	case 'R':
		angle += 25.0f;
		cubeRotation = mat4::RotY(angle);
		break;
	case VK_RBUTTON:
		angle -= 25.0f;
		cubeRotation = mat4::RotY(angle);
		break;
	}*/
}

class Test : public App
{
public:
	void Init() override
	{
		WndInfo wndData{};
		wndData.wndName = "Test";
		wndData.width = 1080;
		wndData.height = 720;
		CreateWindow(&wndData, &gMainWindow);

		VulkanInit(&gVulkan, &gMainWindow, "Test");

		VulkanQueueInfo queueInfo{};
		queueInfo.queueType = QUEUE_TYPE_GRAPHICS;
		queueInfo.index = 0;
		VulkanGetQueueHandle(&gVulkan, &queueInfo, &gGraphicsQueue);

		VulkanSwapChainInfo swapChainInfo{};
		swapChainInfo.window = &gMainWindow;
		swapChainInfo.width = GetWidth(&gMainWindow);
		swapChainInfo.height = GetHeight(&gMainWindow);
		swapChainInfo.format = TinyImageFormat_R8G8B8A8_SRGB;
		swapChainInfo.clearValue.r = 0.0f;
		swapChainInfo.clearValue.g = 0.0f;
		swapChainInfo.clearValue.b = 0.0f;
		swapChainInfo.clearValue.a = 1.0f;

		VulkanCreateSwapChain(&gVulkan, &swapChainInfo, &gSwapChain);

		VulkanRenderAttachmentInfo dbInfo{};
		dbInfo.format = TinyImageFormat_D32_SFLOAT;
		dbInfo.width = gSwapChain.width;
		dbInfo.height = gSwapChain.height;
		dbInfo.clearValue.depth = 1.0f;
		dbInfo.clearValue.stencil = 0;
		VulkanCreateRenderingAttachment(&gVulkan, &dbInfo, &gDepthBuffer);

		VulkanShaderInfo vertexShaderInfo{};
		vertexShaderInfo.filename = "triangle_vs.spv";
		vertexShaderInfo.type = SHADER_TYPE_VERTEX;
		VulkanCreateShader(&gVulkan, &vertexShaderInfo, &gVertexShader);

		VulkanShaderInfo pixelShaderInfo{};
		pixelShaderInfo.filename = "triangle_ps.spv";
		pixelShaderInfo.type = SHADER_TYPE_PIXEL;
		VulkanCreateShader(&gVulkan, &pixelShaderInfo, &gPixelShader);

		VulkanVertexBindingInfo bindingInfo{};
		bindingInfo.binding = 0;
		bindingInfo.stride = sizeof(Vertex);
		bindingInfo.inputRate = INPUT_RATE_PER_VERTEX;

		VulkanAttributeInfo attribInfo[3]{};
		attribInfo[0].binding = 0;
		attribInfo[0].location = 0;
		attribInfo[0].format = TinyImageFormat_R32G32B32A32_SFLOAT;
		attribInfo[0].offset = 0;

		attribInfo[1].binding = 0;
		attribInfo[1].location = 1;
		attribInfo[1].format = TinyImageFormat_R32G32B32A32_SFLOAT;
		attribInfo[1].offset = 16;

		attribInfo[2].binding = 0;
		attribInfo[2].location = 2;
		attribInfo[2].format = TinyImageFormat_R32G32_SFLOAT;
		attribInfo[2].offset = 32;

		VulkanDescritporSetLayoutInfo graphicsLayoutInfo{};
		graphicsLayoutInfo.bindingInfo[0].binding = 0;
		graphicsLayoutInfo.bindingInfo[0].type = DESCRITPTOR_TYPE_UNIFORM_BUFFER;
		graphicsLayoutInfo.bindingInfo[0].stages = STAGE_VERTEX;
		graphicsLayoutInfo.bindingInfo[0].numDescriptors = 1;

		graphicsLayoutInfo.bindingInfo[1].binding = 1;
		graphicsLayoutInfo.bindingInfo[1].type = DESCRITPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		graphicsLayoutInfo.bindingInfo[1].stages = STAGE_PIXEL;
		graphicsLayoutInfo.bindingInfo[1].numDescriptors = 1;

		graphicsLayoutInfo.numBindings = 2;
		VulkanCreateDescriptorSetLayout(&gVulkan, &graphicsLayoutInfo, &gGraphicsLayout);

		VulkanPipleineInfo graphicsPipelineInfo{};
		graphicsPipelineInfo.type = PIPELINE_TYPE_GRAPHICS;
		graphicsPipelineInfo.topology = TOPOLOGY_TRIANGLE_LIST;
		graphicsPipelineInfo.rasInfo.cullMode = CULL_MODE_BACK;
		graphicsPipelineInfo.rasInfo.faceMode = FACE_CLOCKWISE;
		graphicsPipelineInfo.rasInfo.polygonMode = POLYGON_MODE_FILL;
		graphicsPipelineInfo.rasInfo.lineWidth = 1.0f;
		graphicsPipelineInfo.blendInfo.enableBlend = false;
		graphicsPipelineInfo.shaders[0] = gVertexShader;
		graphicsPipelineInfo.shaders[1] = gPixelShader;
		graphicsPipelineInfo.colorFormat = gSwapChain.format;
		graphicsPipelineInfo.depthInfo.depthTest = true;
		graphicsPipelineInfo.depthInfo.depthWrite = true;
		graphicsPipelineInfo.depthInfo.depthFunction = DEPTH_FUNCTION_LESS;
		graphicsPipelineInfo.depthFormat = TinyImageFormat_D32_SFLOAT;
		graphicsPipelineInfo.bindingInfo = bindingInfo;
		graphicsPipelineInfo.attributeInfo[0] = attribInfo[0];
		graphicsPipelineInfo.attributeInfo[1] = attribInfo[1];
		graphicsPipelineInfo.attributeInfo[2] = attribInfo[2];
		graphicsPipelineInfo.numAttributes = 3;
		graphicsPipelineInfo.descriptorSetLayout = &gGraphicsLayout;

		VulkanCreatePipeline(&gVulkan, &graphicsPipelineInfo, &gGraphicsPipeline);

		VulkanShaderInfo computeShaderInfo{};
		computeShaderInfo.filename = "cube_cs.spv";
		computeShaderInfo.type = SHADER_TYPE_COMPUTE;
		VulkanCreateShader(&gVulkan, &computeShaderInfo, &gComputeShader);

		VulkanDescritporSetLayoutInfo computeLayoutInfo{};

		computeLayoutInfo.bindingInfo[0].binding = 0;
		computeLayoutInfo.bindingInfo[0].type = DESCRITPTOR_TYPE_STORAGE_BUFFER;
		computeLayoutInfo.bindingInfo[0].stages = STAGE_COMPUTE;
		computeLayoutInfo.bindingInfo[0].numDescriptors = 1;

		computeLayoutInfo.bindingInfo[1].binding = 1;
		computeLayoutInfo.bindingInfo[1].type = DESCRITPTOR_TYPE_STORAGE_BUFFER;
		computeLayoutInfo.bindingInfo[1].stages = STAGE_COMPUTE;
		computeLayoutInfo.bindingInfo[1].numDescriptors = 1;

		computeLayoutInfo.numBindings = 2;
		VulkanCreateDescriptorSetLayout(&gVulkan, &computeLayoutInfo, &gComputeLayout);

		VulkanPipleineInfo computePipelineInfo{};
		computePipelineInfo.type = PIPELINE_TYPE_COMPUTE;
		computePipelineInfo.shaders[0] = gComputeShader;
		computePipelineInfo.descriptorSetLayout = &gComputeLayout;
		VulkanCreatePipeline(&gVulkan, &computePipelineInfo, &gComputePipeline);

		VulkanCreateCommandPool(&gVulkan, &gCommandPool, QUEUE_TYPE_GRAPHICS);

		for (uint32_t i = 0; i < gNumFrames; ++i)
		{
			VulkanCreateCommandBuffer(&gVulkan, &gCommandPool, &gCommandBuffers[i], 1);

			VulkanCreateSemaphore(&gVulkan, &gImageAvailableSemaphores[i]);
			VulkanCreateSemaphore(&gVulkan, &gRenderFinishedSemaphores[i]);

			VulkanCreateFence(&gVulkan, &gFences[i]);
		}

		VulkanBufferInfo vbInfo{};
		vbInfo.size = 8 * sizeof(Vertex);
		vbInfo.type = BUFFER_TYPE_VERTEX_BUFFER | BUFFER_TYPE_STORAGE_BUFFER;
		vbInfo.access = ACCESS_GPU;
		vbInfo.data = nullptr;

		VulkanCreateBuffer(&gVulkan, &vbInfo, &gVertexBuffer);

		VulkanBufferInfo ibInfo{};
		ibInfo.size = 36 * sizeof(uint32_t);
		ibInfo.type = BUFFER_TYPE_INDEX_BUFFER | BUFFER_TYPE_STORAGE_BUFFER;
		ibInfo.access = ACCESS_GPU;
		ibInfo.indexType = INDEX_TYPE_UINT32;
		ibInfo.data = nullptr;

		VulkanCreateBuffer(&gVulkan, &ibInfo, &gIndexBuffer);

		for (uint32_t i = 0; i < gNumFrames; ++i)
		{
			VulkanBufferInfo ubInfo{};
			ubInfo.size = sizeof(UniformData);
			ubInfo.type = BUFFER_TYPE_UNIFORM_BUFFER;
			ubInfo.access = ACCESS_CPU_GPU;
			ubInfo.data = nullptr;

			VulkanCreateBuffer(&gVulkan, &ubInfo, &gUniformBuffer[i]);
		}

		VulkanCreateTexture(&gVulkan, "statue.dds", &gStatueTexture);

		VulkanSamplerInfo samplerInfo{};
		samplerInfo.magFilter = LINEAR_FILTER;
		samplerInfo.minFilter = LINEAR_FILTER;
		samplerInfo.u = ADDRESS_MODE_REPEAT;
		samplerInfo.v = ADDRESS_MODE_REPEAT;
		samplerInfo.w = ADDRESS_MODE_REPEAT;
		samplerInfo.mipMapMode = MIPMAP_MODE_LINEAR;
		samplerInfo.maxAnisotropy = 1.0f;
		samplerInfo.mipLoadBias = 0.0f;
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = 0.0f;
		VulkanCreateSampler(&gVulkan, &samplerInfo, &gStatueSampler);
		
		VulkanDescriptorSetInfo uniformSetInfo{};
		VulkanDescriptorSetLayout layouts[2] = { gGraphicsLayout , gGraphicsLayout };
		uniformSetInfo.layouts = layouts;
		uniformSetInfo.numDescriptorSets = gNumFrames;
		VulkanCreateDescriptorSet(&gVulkan, &uniformSetInfo, &gGraphicsDescriptorSet);

		for (uint32_t i = 0; i < gNumFrames; ++i)
		{
			VulkanUpdateDescriptorSetInfo updateUniformSetInfo{};
			updateUniformSetInfo.index = i;
			updateUniformSetInfo.binding = 0;
			updateUniformSetInfo.bufferInfo.buffer = &gUniformBuffer[i];
			updateUniformSetInfo.bufferInfo.offset = 0;
			updateUniformSetInfo.bufferInfo.size = sizeof(UniformData);
			updateUniformSetInfo.bufferInfo.type = DESCRITPTOR_TYPE_UNIFORM_BUFFER;

			VulkanUpdateDescriptorSet(&gVulkan, &updateUniformSetInfo, &gGraphicsDescriptorSet);

			VulkanUpdateDescriptorSetInfo updateImageSetInfo{};
			updateImageSetInfo.index = i;
			updateImageSetInfo.binding = 1;
			updateImageSetInfo.imageInfo.texture = &gStatueTexture;
			updateImageSetInfo.imageInfo.sampler = &gStatueSampler;

			VulkanUpdateDescriptorSet(&gVulkan, &updateImageSetInfo, &gGraphicsDescriptorSet);
		}

		VulkanDescriptorSetInfo computeSetInfo{};
		computeSetInfo.layouts = &gComputeLayout;
		computeSetInfo.numDescriptorSets = 1;
		VulkanCreateDescriptorSet(&gVulkan, &computeSetInfo, &gComputeDescriptorSet);

		VulkanUpdateDescriptorSetInfo updateComputeSetInfo{};
		updateComputeSetInfo.index = 0;
		updateComputeSetInfo.binding = 0;
		updateComputeSetInfo.bufferInfo.buffer = &gVertexBuffer;
		updateComputeSetInfo.bufferInfo.offset = 0;
		updateComputeSetInfo.bufferInfo.size = 8 * sizeof(Vertex);
		updateComputeSetInfo.bufferInfo.type = DESCRITPTOR_TYPE_STORAGE_BUFFER;
		VulkanUpdateDescriptorSet(&gVulkan, &updateComputeSetInfo, &gComputeDescriptorSet);

		updateComputeSetInfo.index = 0;
		updateComputeSetInfo.binding = 1;
		updateComputeSetInfo.bufferInfo.buffer = &gIndexBuffer;
		updateComputeSetInfo.bufferInfo.offset = 0;
		updateComputeSetInfo.bufferInfo.size = 36 * sizeof(uint32_t);
		updateComputeSetInfo.bufferInfo.type = DESCRITPTOR_TYPE_STORAGE_BUFFER;
		VulkanUpdateDescriptorSet(&gVulkan, &updateComputeSetInfo, &gComputeDescriptorSet);

		LookAt(&gCamera, vec3(3.0f, 2.0f, -6.0f), vec3(3.0f, 2.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f));

		gCamera.vFov = 45.0f;
		gCamera.nearP = 1.0f;
		gCamera.farP = 100.0f;

		gCubeRotation = mat4::MakeIdentity();

		gFuncOnKeyReleased = OnKeyReleased;
		gOnResize = OnResize;

		InitTimer(&gTimer);

		VulkanResetCommandBuffer(&gCommandBuffers[gCurrentFrame]);

		VulkanBeginCommandBuffer(&gCommandBuffers[gCurrentFrame]);

		VulkanBindPipeline(&gCommandBuffers[gCurrentFrame], &gComputePipeline);

		VulkanBindDescriptorSet(&gCommandBuffers[gCurrentFrame], &gComputePipeline, 0, &gComputeDescriptorSet);

		VulkanDispatch(&gCommandBuffers[gCurrentFrame], 1, 1, 1);

		VulkanEndCommandBuffer(&gCommandBuffers[gCurrentFrame]);

		VulkanQueueSubmitInfo submitInfo{};
		submitInfo.queue = &gGraphicsQueue;
		submitInfo.numWaitSemaphores = 0;
		submitInfo.waitSemaphores = nullptr;
		submitInfo.numSignalSemaphores = 0;
		submitInfo.signalSemaphores = nullptr;
		submitInfo.fence = nullptr;
		submitInfo.commandBuffer = &gCommandBuffers[gCurrentFrame];
		VulkanQueueSubmit(&gVulkan, &submitInfo);

		VulkanWaitQueueIdle(&gGraphicsQueue);
	}

	void Exit() override
	{
		VulkanWaitDeviceIdle(&gVulkan);

		VulkanDestroyDescriptorSet(&gGraphicsDescriptorSet);
		VulkanDestroyDescriptorSet(&gComputeDescriptorSet);

		VulkanDestroySampler(&gVulkan, &gStatueSampler);
		VulkanDestroyTexture(&gVulkan, &gStatueTexture);

		for (uint32_t i = 0; i < gNumFrames; ++i)
		{
			VulkanDestroyBuffer(&gVulkan, &gUniformBuffer[i]);
		}

		for (uint32_t i = 0; i < gNumFrames; ++i)
		{
			VulkanDestroySemaphore(&gVulkan, &gImageAvailableSemaphores[i]);
			VulkanDestroySemaphore(&gVulkan, &gRenderFinishedSemaphores[i]);
			VulkanDestroyFence(&gVulkan, &gFences[i]);
		}

		VulkanDestroyCommandPool(&gVulkan, &gCommandPool);

		VulkanDestroyPipeline(&gVulkan, &gComputePipeline);
		VulkanDestroyPipeline(&gVulkan, &gGraphicsPipeline);

		VulkanDestroyDescriptorSetLayout(&gVulkan, &gComputeLayout);
		VulkanDestroyDescriptorSetLayout(&gVulkan, &gGraphicsLayout);

		VulkanDestroyBuffer(&gVulkan, &gIndexBuffer);
		VulkanDestroyBuffer(&gVulkan, &gVertexBuffer);

		VulkanDestroyShader(&gVulkan, &gComputeShader);
		VulkanDestroyShader(&gVulkan, &gPixelShader);
		VulkanDestroyShader(&gVulkan, &gVertexShader);

		VulkanDestroyRenderingAttachment(&gVulkan, &gDepthBuffer);

		VulkanDestroySwapChain(&gVulkan, &gSwapChain);

		VulkanDestroy(&gVulkan);
	}

	void UserInput() override
	{
		static float moveSpeed = 0.025f;
		static float turnSpeed = 0.25f;
		static vec2 lastMousePosition;

		if (CheckKeyDown('W'))
			MoveForward(&gCamera, moveSpeed);
		if (CheckKeyDown('S'))
			MoveBackward(&gCamera, moveSpeed);
		if (CheckKeyDown('A'))
			MoveLeft(&gCamera, moveSpeed);
		if (CheckKeyDown('D'))
			MoveRight(&gCamera, moveSpeed);
		if (CheckKeyDown('Q'))
			MoveUp(&gCamera, moveSpeed);
		if (CheckKeyDown('E'))
			MoveDown(&gCamera, moveSpeed);
		if(CheckKeyDown(VK_SPACE))
			LookAt(&gCamera, vec3(0.0f, 0.0f, -2.0f), vec3(0.0f, 0.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f));
		if (CheckKeyDown('L'))
			RotateCamera(&gCamera, quat::MakeRotation(turnSpeed, vec3(0.0f, 1.0f, 0.0f)));
		if (CheckKeyDown('J'))
			RotateCamera(&gCamera, quat::MakeRotation(-turnSpeed, vec3(0.0f, 1.0f, 0.0f)));
		if (CheckKeyDown('I'))
			RotateCamera(&gCamera, quat::MakeRotation(turnSpeed, gCamera.right));
		if (CheckKeyDown('K'))
			RotateCamera(&gCamera, quat::MakeRotation(-turnSpeed, gCamera.right));

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

	void Update() override
	{
		gCamera.aspectRatio = (float)GetWidth(&gMainWindow) / GetHeight(&gMainWindow);
		UpdateViewMatrix(&gCamera);
		UpdatePerspectiveProjectionMatrix(&gCamera);

		float x = 0.0f;
		float y = 0.0f;
		gAngle += 25.0f * gTimer.deltaTime;
		if (gAngle >= 360.0f)
			gAngle = 0.0f;
		gCubeRotation = mat4::RotY(gAngle);
		for (uint32_t i = 0; i < 12; ++i)
		{
			if (i != 0 && i % 4 == 0)
			{
				y += 2.0f;
				x = 0.0f;
			}
			gUniformData.model[i] = mat4::Scale(1.0f, 1.0f, 1.0f) * gCubeRotation * mat4::Translate(x, y, 0.0f);
			gUniformData.view[i] = gCamera.viewMat;
			gUniformData.projection[i] = gCamera.perspectiveProjMat;
			gUniformData.projection[i].SetElement(1, 1, gUniformData.projection[i].GetElement(1, 1) * -1.0f);
			x += 2.0f;
		}

	}

	void Draw() override
	{
		VulkanWaitForFence(&gVulkan, &gFences[gCurrentFrame]);

		void* data = nullptr;
		VulkanMapMemory(&gVulkan, &gUniformBuffer[gCurrentFrame], &data);
		memcpy(data, &gUniformData, sizeof(gUniformData));
		VulkanUnmapMemory(&gVulkan, &gUniformBuffer[gCurrentFrame]);

		uint32_t imageIndex = 0;
		VulkanAcquireNextImage(&gVulkan, &gSwapChain, &gImageAvailableSemaphores[gCurrentFrame], &imageIndex);

		VulkanResetCommandBuffer(&gCommandBuffers[gCurrentFrame]);

		VulkanBeginCommandBuffer(& gCommandBuffers[gCurrentFrame]);

		VulkanTransitionImage(&gCommandBuffers[gCurrentFrame], &gSwapChain.colorAttachments[imageIndex],
			LAYOUT_UNDEFINED, LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

		VulkanBindRenderingAttachmentInfo attachmentInfo{};
		attachmentInfo.colorAttachment = &gSwapChain.colorAttachments[imageIndex];
		attachmentInfo.colorAttachmentLoadOp = LOAD_OP_CLEAR;
		attachmentInfo.colorAttachmentStoreOp = STORE_OP_STORE;
		attachmentInfo.depthAttachment = &gDepthBuffer;
		attachmentInfo.depthAttachmentLoadOp = LOAD_OP_CLEAR;
		attachmentInfo.depthAttachmentStoreOp = STORE_OP_DONT_CARE;
		VulkanBindRenderAttachment(&gCommandBuffers[gCurrentFrame], &attachmentInfo);

		ViewportInfo viewportInfo{};
		viewportInfo.x = 0.0f;
		viewportInfo.y = 0.0f;
		viewportInfo.width = gSwapChain.width;
		viewportInfo.height = gSwapChain.height;
		viewportInfo.minDepth = 0.0f;
		viewportInfo.maxDepth = 1.0f;
		VulkanSetViewport(&gCommandBuffers[gCurrentFrame], &viewportInfo);

		ScissorInfo scissorInfo{};
		scissorInfo.x = 0.0f;
		scissorInfo.y = 0.0f;
		scissorInfo.width = gSwapChain.width;
		scissorInfo.height = gSwapChain.height;
		VulkanSetScissor(&gCommandBuffers[gCurrentFrame], &scissorInfo);

		VulkanBindBuffer(&gCommandBuffers[gCurrentFrame], 0, 0, &gVertexBuffer);
		VulkanBindBuffer(&gCommandBuffers[gCurrentFrame], 0, 0, &gIndexBuffer);

		VulkanBindPipeline(&gCommandBuffers[gCurrentFrame], &gGraphicsPipeline);

		VulkanBindDescriptorSet(&gCommandBuffers[gCurrentFrame], &gGraphicsPipeline, gCurrentFrame, &gGraphicsDescriptorSet);
		VulkanDrawIndexed(&gCommandBuffers[gCurrentFrame], 36, 12, 0, 0, 0);

		VulkanBindRenderAttachment(&gCommandBuffers[gCurrentFrame], nullptr);

		VulkanTransitionImage(&gCommandBuffers[gCurrentFrame], &gSwapChain.colorAttachments[imageIndex],
			LAYOUT_COLOR_ATTACHMENT_OPTIMAL, LAYOUT_PRESENT_SRC);

		VulkanEndCommandBuffer(&gCommandBuffers[gCurrentFrame]);

		VulkanQueueSubmitInfo submitInfo{};
		submitInfo.queue = &gGraphicsQueue;
		submitInfo.numWaitSemaphores = 1;
		submitInfo.waitSemaphores = &gImageAvailableSemaphores[gCurrentFrame];
		submitInfo.numSignalSemaphores = 1;
		submitInfo.signalSemaphores = &gRenderFinishedSemaphores[gCurrentFrame];
		submitInfo.fence = &gFences[gCurrentFrame];
		submitInfo.commandBuffer = &gCommandBuffers[gCurrentFrame];
		VulkanQueueSubmit(&gVulkan, &submitInfo);

		VulkanPresentInfo presentInfo{};
		presentInfo.queue = &gGraphicsQueue;
		presentInfo.waitSemaphore = &gRenderFinishedSemaphores[gCurrentFrame];
		presentInfo.swapChain = &gSwapChain;
		presentInfo.imageIndex = imageIndex;
		VulkanQueuePresent(&gVulkan, &presentInfo);

		gCurrentFrame = (gCurrentFrame + 1) % gNumFrames;
	}
};

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	Test test;

	test.Init();

	MSG msg{};
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			Tick(&gTimer);

			if (!gAppPaused)
			{
				test.UserInput();
				test.Update();
				test.Draw();
			}
		}
		
	}

	test.Exit();
	return 0;
};
