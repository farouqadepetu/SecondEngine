#include "SEApp.h"
#include "SEVulkan.h"
#include "math_header.h"
#include "SECamera.h"

SEWindow gMainWindow;

SEVulkan gVulkan;

SEVulkanQueue gGraphicsQueue;

SEVulkanSwapChain gSwapChain;

SEVulkanShader gVertexShader;
SEVulkanShader gPixelShader;
SEVulkanDescriptorSetLayout gLayout;
SEVulkanPipeline gPipeline;

SEVulkanCommandPool gCommandPool;

const uint32_t gNumFrames = 2;
SEVulkanCommandBuffer gCommandBuffers[gNumFrames];
SEVulkanSemaphore gImageAvailableSemaphores[gNumFrames];
SEVulkanSemaphore gRenderFinishedSemaphores[gNumFrames];
SEVulkanFence gFences[gNumFrames];

SEVulkanBuffer gVertexBuffer;
SEVulkanBuffer gIndexBuffer;

SEVulkanBuffer gUniformBuffer[gNumFrames];

uint32_t gCurrentFrame = 0;

SEVulkanDescriptorSet gDescriptorSets[gNumFrames];

SECamera gCamera;

struct Vertex
{
	vec4 postion;
	vec4 color;
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
	{{vec4(0.5f, 0.5f, 0.0f, 1.0f)}, {vec4(1.0f, 0.0f, 0.0f, 1.0f)}},
	{{vec4(0.5f, -0.5f, 0.0f, 1.0f)}, {vec4(0.0f, 1.0f, 0.0f, 1.0f)}},
	{{vec4(-0.5f, 0.5f, 0.0f, 1.0f)}, {vec4(0.0f, 0.0f, 1.0f, 1.0f)}},
	{{vec4(-0.5f, -0.5f, 0.0f, 1.0f)}, {vec4(1.0f, 1.0f, 1.0f, 1.0f)}}
};

const uint32_t quadIndices[6] = { 0, 1, 2, 3, 2, 1 };

struct UniformData
{
	mat4 model;
	mat4 view;
	mat4 projection;
}gUniformData;

void OnResize()
{
	VulkanOnResize(&gVulkan, &gMainWindow, &gSwapChain, &gPipeline);
}

class Test : public App
{
public:
	void Init() override
	{
		SEWndInfo wndData{};
		wndData.wndName = "Test";
		wndData.width = 1080;
		wndData.height = 720;
		CreateSEWindow(&wndData, &gMainWindow);

		InitVulkan(&gVulkan, &gMainWindow, "Test");

		SEVulkanQueueInfo queueInfo{};
		queueInfo.queueType = SE_GRAPHICS_QUEUE;
		queueInfo.index = 0;
		GetQueueHandle(&gVulkan, &queueInfo, &gGraphicsQueue);

		CreateVulkanSwapChain(&gVulkan, &gMainWindow, &gSwapChain);

		CreateVulkanShader(&gVulkan, "triangle_vs.spv", SE_VERTEX_SHADER, &gVertexShader);
		CreateVulkanShader(&gVulkan, "triangle_ps.spv", SE_PIXEL_SHADER, &gPixelShader);

		SEVulkanVertexBindingInfo bindingInfo{};
		bindingInfo.binding = 0;
		bindingInfo.stride = sizeof(Vertex);
		bindingInfo.inputRate = SE_PER_VERTEX;

		SEVulkanAttributeInfo attribInfo[2]{};
		attribInfo[0].binding = 0;
		attribInfo[0].location = 0;
		attribInfo[0].format = TinyImageFormat_R32G32B32A32_SFLOAT;
		attribInfo[0].offset = 0;

		attribInfo[1].binding = 0;
		attribInfo[1].location = 1;
		attribInfo[1].format = TinyImageFormat_R32G32B32A32_SFLOAT;
		attribInfo[1].offset = 16;

		SEVulkanDescritporSetLayoutInfo layoutInfo{};
		layoutInfo.bindingInfo[0].binding = 0;
		layoutInfo.bindingInfo[0].type = SE_DESCRITPTOR_UNIFORM_BUFFER;
		layoutInfo.bindingInfo[0].stages = SE_VERTEX_STAGE;
		layoutInfo.bindingInfo[0].numDescriptors = 1;

		layoutInfo.numBindings = 1;

		CreateVulkanDescriptorSetLayout(&gVulkan, &layoutInfo, &gLayout);

		SEVulkanPipleineInfo pipelineInfo{};
		pipelineInfo.topology = SE_TRIANGLE_LIST;
		pipelineInfo.rasInfo.cullMode = SE_CULL_BACK;
		pipelineInfo.rasInfo.faceMode = SE_FACE_CLOCKWISE;
		pipelineInfo.rasInfo.lineWidth = 1.0f;
		pipelineInfo.shaders[0] = gVertexShader;
		pipelineInfo.shaders[1] = gPixelShader;
		pipelineInfo.swapChainFormat = gSwapChain.format;
		pipelineInfo.bindingInfo = bindingInfo;
		pipelineInfo.attributeInfo[0] = attribInfo[0];
		pipelineInfo.attributeInfo[1] = attribInfo[1];
		pipelineInfo.numAttributes = 2;
		pipelineInfo.descriptorSetLayout = &gLayout;

		CreateVulkanPipeline(&gVulkan, &pipelineInfo, &gPipeline);

		CreateVulkanFrameBuffer(&gVulkan, &gSwapChain, &gPipeline.renderPass);

		CreateVulkanCommandPool(&gVulkan, &gCommandPool, SE_GRAPHICS_QUEUE);

		for (uint32_t i = 0; i < gNumFrames; ++i)
		{
			CreateVulkanCommandBuffer(&gVulkan, &gCommandPool, &gCommandBuffers[i], 1);

			CreateVulkanSemaphore(&gVulkan, &gImageAvailableSemaphores[i]);
			CreateVulkanSemaphore(&gVulkan, &gRenderFinishedSemaphores[i]);

			CreateVulkanFence(&gVulkan, &gFences[i]);
		}

		SEVulkanBufferInfo vbInfo{};
		vbInfo.size = 4 * sizeof(Vertex);
		vbInfo.type = SE_VERTEX_BUFFER;
		vbInfo.access = SE_GPU;
		vbInfo.data = (void*)quadVertices;

		CreateVulkanBuffer(&gVulkan, &vbInfo, &gVertexBuffer);

		SEVulkanBufferInfo ibInfo{};
		ibInfo.size = 6 * sizeof(uint32_t);
		ibInfo.type = SE_INDEX_BUFFER;
		ibInfo.access = SE_GPU;
		ibInfo.data = (void*)quadIndices;

		CreateVulkanBuffer(&gVulkan, &ibInfo, &gIndexBuffer);

		for (uint32_t i = 0; i < gNumFrames; ++i)
		{
			SEVulkanBufferInfo ubInfo{};
			ubInfo.size = sizeof(UniformData);
			ubInfo.type = SE_UNIFORM_BUFFER;
			ubInfo.access = SE_CPU_GPU;
			ubInfo.data = nullptr;

			CreateVulkanBuffer(&gVulkan, &ubInfo, &gUniformBuffer[i]);
		}

		SEVulkanDescriptorSetInfo setInfo{};
		setInfo.layout = &gLayout;
		setInfo.type = SE_DESCRITPTOR_UNIFORM_BUFFER;
		setInfo.numDescriptors = gNumFrames;

		CreateVulkanDescriptorSets(&gVulkan, &setInfo, gDescriptorSets);
		
		for (uint32_t i = 0; i < gNumFrames; ++i)
		{
			SEVulkanUpdateDescriptorSetInfo updateSetInfo{};
			updateSetInfo.binding = 0;
			updateSetInfo.firstArrayElement = 0;
			updateSetInfo.numArrayElements = 1;
			updateSetInfo.bufferInfo.buffer = &gUniformBuffer[i];
			updateSetInfo.bufferInfo.offset = 0;
			updateSetInfo.bufferInfo.range = sizeof(UniformData);

			UpdateVulkanDescriptorSet(&gVulkan, &updateSetInfo, &gDescriptorSets[i]);
		}

		LookAt(&gCamera, vec3(0.0f, 2.0f, -2.0f), vec3(0.0f, 0.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f));
		 
		gCamera.vFov = 45.0f;
		gCamera.nearP = 1.0f;
		gCamera.farP = 100.0f;

	}

	void Exit() override
	{
		VulkanWaitDeviceIdle(&gVulkan);

		for (uint32_t i = 0; i < gNumFrames; ++i)
		{
			DestroyVulkanBuffer(&gVulkan, &gUniformBuffer[i]);
		}

		for (uint32_t i = 0; i < gNumFrames; ++i)
		{
			DestroyVulkanSemaphore(&gVulkan, &gImageAvailableSemaphores[i]);
			DestroyVulkanSemaphore(&gVulkan, &gRenderFinishedSemaphores[i]);
			DestroyVulkanFence(&gVulkan, &gFences[i]);
		}

		DestroyVulkanCommandPool(&gVulkan, &gCommandPool);

		DestroyVulkanPipeline(&gVulkan, &gPipeline);
		DestroyVulkanDescriptorSetLayout(&gVulkan, &gLayout);
		DestroyVulkanBuffer(&gVulkan, &gIndexBuffer);
		DestroyVulkanBuffer(&gVulkan, &gVertexBuffer);
		DestroyVulkanShader(&gVulkan, &gVertexShader);
		DestroyVulkanShader(&gVulkan, &gPixelShader);
		DestroyVulkanSwapChain(&gVulkan, &gSwapChain);
		DestroyVulkan(&gVulkan);
	}

	void UserInput() override
	{

	}

	void Update() override
	{
		gCamera.aspectRatio = (float)GetWidth(&gMainWindow) / GetHeight(&gMainWindow);
		UpdateViewMatrix(&gCamera);
		UpdatePerspectiveProjectionMatrix(&gCamera);

		gUniformData.model = mat4::Scale(1.0f, 1.0f, 1.0f) * mat4::RotX(90.0f) * mat4::Translate(0.0f, 0.0f, 2.0f);
		gUniformData.view = gCamera.viewMat;
		gUniformData.projection = gCamera.perspectiveProjMat;
		gUniformData.projection.SetElement(1, 1, gUniformData.projection.GetElement(1, 1) * -1.0f);

	}

	void Draw() override
	{
		VulkanWaitForFence(&gVulkan, &gFences[gCurrentFrame]);

		void* data = nullptr;
		VulkanMapMemory(&gVulkan, &gUniformBuffer[gCurrentFrame], 0, sizeof(gUniformData), &data);
		memcpy(data, &gUniformData, sizeof(gUniformData));
		VulkanUnmapMemory(&gVulkan, &gUniformBuffer[gCurrentFrame]);

		uint32_t imageIndex = 0;
		VulkanAcquireNextImage(&gVulkan, &gSwapChain, &gImageAvailableSemaphores[gCurrentFrame], &imageIndex);

		VulkanResetCommandBuffer(&gCommandBuffers[gCurrentFrame]);

		VulkanBeginCommandBuffer(&gVulkan, &gCommandBuffers[gCurrentFrame], &gPipeline, &gSwapChain, imageIndex);

		VulkanBindPipeline(&gCommandBuffers[gCurrentFrame], &gPipeline, SE_GRAPHICS_PIPELINE);

		SEViewportInfo viewportInfo{};
		viewportInfo.x = 0.0f;
		viewportInfo.y = 0.0f;
		viewportInfo.width = gSwapChain.extent.width;
		viewportInfo.height = gSwapChain.extent.height;
		viewportInfo.minDepth = 0.0f;
		viewportInfo.maxDepth = 1.0f;
		VulkanSetViewport(&gCommandBuffers[gCurrentFrame], &viewportInfo);

		SEScissorInfo scissorInfo{};
		scissorInfo.x = 0.0f;
		scissorInfo.y = 0.0f;
		scissorInfo.width = gSwapChain.extent.width;
		scissorInfo.height = gSwapChain.extent.height;
		VulkanSetScissor(&gCommandBuffers[gCurrentFrame], &scissorInfo);

		VulkanBindBuffer(&gCommandBuffers[gCurrentFrame], 0, &gVertexBuffer, 0, SE_VERTEX_BUFFER);
		VulkanBindBuffer(&gCommandBuffers[gCurrentFrame], 0, &gIndexBuffer, 0, SE_INDEX_BUFFER);

		VulkanBindDescriptorSet(&gCommandBuffers[gCurrentFrame], SE_GRAPHICS_PIPELINE, &gPipeline, &gDescriptorSets[gCurrentFrame]);
		VulkanDrawIndexed(&gCommandBuffers[gCurrentFrame], 6, 1, 0, 0, 0);

		VulkanEndCommandBuffer(&gCommandBuffers[gCurrentFrame]);

		VulkanQueueSubmitInfo submitInfo{};
		submitInfo.queue = &gGraphicsQueue;
		submitInfo.waitSemaphore = &gImageAvailableSemaphores[gCurrentFrame];
		submitInfo.signalSemaphore = &gRenderFinishedSemaphores[gCurrentFrame];
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
			if (!gAppPaused)
			{
				test.Update();
				test.Draw();
			}
		}
		
	}

	test.Exit();
	return 0;
};
