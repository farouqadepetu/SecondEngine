#include "SEApp.h"
#include "math_header.h"

SEWindow gMainWindow;

SEVulkan gVulkan;

SEVulkanQueue gGraphicsQueue;

SEVulkanSwapChain gSwapChain;

SEVulkanShader gVertexShader;
SEVulkanShader gPixelShader;
SEVulkanPipeline gPipeline;

SEVulkanCommandPool gCommandPool;

const uint32_t gNumFrames = 2;
SEVulkanCommandBuffer gCommandBuffers[gNumFrames];
SEVulkanSemaphore gImageAvailableSemaphores[gNumFrames];
SEVulkanSemaphore gRenderFinishedSemaphores[gNumFrames];
SEVulkanFence gFences[gNumFrames];

uint32_t gCurrentFrame = 0;

struct Vertex
{
	vec4 postion;
	vec4 color;
};

const Vertex triangleVertices[3] =
{ 
	{{vec4(0.0f, -0.5f, 0.0f, 1.0f)}, {vec4(1.0f, 0.0f, 0.0f, 1.0f)}},
	{{vec4(0.5f, 0.5f, 0.0f, 1.0f)}, {vec4(0.0f, 1.0f, 0.0f, 1.0f)}},
	{{vec4(-0.5f, 0.5f, 0.0f, 1.0f)}, {vec4(0.0f, 0.0f, 1.0f, 1.0f)}}
};

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
		queueInfo.queueType = GRAPHICS_QUEUE;
		queueInfo.index = 0;
		GetQueueHandle(&gVulkan, &queueInfo, &gGraphicsQueue);

		CreateVulkanSwapChain(&gVulkan, &gMainWindow, &gSwapChain);

		CreateVulkanShader(&gVulkan, "triangle_vs.spv", VERTEX, &gVertexShader);
		CreateVulkanShader(&gVulkan, "triangle_ps.spv", PIXEL, &gPixelShader);

		SEVulkanVertexBindingInfo bindingInfo{};
		bindingInfo.binding = 0;
		bindingInfo.stride = sizeof(Vertex);
		bindingInfo.inputRate = SE_PER_VERTEX;

		SEVulkanAttributeInfo attribInfo[2]{};
		attribInfo[0].binding = 0;
		attribInfo[0].location = 0;
		attribInfo[0].format = SE_FORMAT_R32G32B32A32_FLOAT;
		attribInfo[0].offset = 0;

		attribInfo[1].binding = 0;
		attribInfo[1].location = 1;
		attribInfo[1].format = SE_FORMAT_R32G32B32A32_FLOAT;
		attribInfo[1].offset = 16;

		SEVulkanPipleineInfo pipelineInfo{};
		pipelineInfo.topology = TRIANGLE_LIST;
		pipelineInfo.rasInfo.cullMode = BACK;
		pipelineInfo.rasInfo.faceMode = CLOCKWISE;
		pipelineInfo.rasInfo.lineWidth = 1.0f;
		pipelineInfo.shaders[0] = gVertexShader;
		pipelineInfo.shaders[1] = gPixelShader;
		pipelineInfo.swapChainFormat = gSwapChain.format;
		pipelineInfo.bindingInfo = bindingInfo;
		pipelineInfo.attributeInfo = attribInfo;

		CreateVulkanPipeline(&gVulkan, &pipelineInfo, &gPipeline);

		CreateVulkanFrameBuffer(&gVulkan, &gSwapChain, &gPipeline.renderPass);

		CreateVulkanCommandPool(&gVulkan, &gCommandPool, GRAPHICS_QUEUE);

		for (uint32_t i = 0; i < gNumFrames; ++i)
		{
			CreateVulkanCommandBuffer(&gVulkan, &gCommandPool, &gCommandBuffers[i], 1);

			CreateVulkanSemaphore(&gVulkan, &gImageAvailableSemaphores[i]);
			CreateVulkanSemaphore(&gVulkan, &gRenderFinishedSemaphores[i]);

			CreateVulkanFence(&gVulkan, &gFences[i]);
		}
	}

	void Exit() override
	{
		VulkanWaitDeviceIdle(&gVulkan);

		for (uint32_t i = 0; i < gNumFrames; ++i)
		{
			DestroyVulkanSemaphore(&gVulkan, &gImageAvailableSemaphores[i]);
			DestroyVulkanSemaphore(&gVulkan, &gRenderFinishedSemaphores[i]);
			DestroyVulkanFence(&gVulkan, &gFences[i]);
		}

		DestroyVulkanCommandPool(&gVulkan, &gCommandPool);

		DestroyVulkanPipeline(&gVulkan, &gPipeline);
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

	}

	void Draw() override
	{
		VulkanWaitForFence(&gVulkan, &gFences[gCurrentFrame]);

		uint32_t imageIndex = 0;
		VulkanAcquireNextImage(&gVulkan, &gSwapChain, &gImageAvailableSemaphores[gCurrentFrame], &imageIndex);

		VulkanResetCommandBuffer(&gCommandBuffers[gCurrentFrame]);

		VulkanBeginCommandBuffer(&gVulkan, &gCommandBuffers[gCurrentFrame], &gPipeline, &gSwapChain, imageIndex);

		VulkanBindPipeline(&gCommandBuffers[gCurrentFrame], &gPipeline, GRAPHICS_PIPELINE);

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

		VulkanDraw(&gCommandBuffers[gCurrentFrame], 3, 1, 0, 0);

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
				test.Draw();
			}
		}
		
	}

	test.Exit();
	return 0;
};
