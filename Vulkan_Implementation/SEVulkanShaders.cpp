#include "SEVulkanShaders.h"

void CreateVulkanShader(SEVulkan* vulk, const char* filename, ShaderType type, SEVulkanShader* shader)
{
	char* buffer = nullptr;
	uint32_t fileSize = 0;
	ReadFile(filename, buffer, &fileSize);
	uint32_t* code = (uint32_t*)calloc(ceil(fileSize / 4.0f), sizeof(uint32_t));
	memcpy(code, buffer, fileSize);
	FreeFileBuffer(buffer);

	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.flags = 0;
	createInfo.codeSize = fileSize;
	createInfo.pCode = code;

	vkCreateShaderModule(vulk->logicalDevice, &createInfo, nullptr, &shader->shaderModule);

	VkPipelineShaderStageCreateInfo stageCreateInfo{};
	stageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	stageCreateInfo.pNext = nullptr;
	stageCreateInfo.flags = 0;
	switch (type)
	{
	case VERTEX:
		stageCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		break;

	case PIXEL:
		stageCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		break;

	case COMPUTE: 
		stageCreateInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
		break;
	}
	stageCreateInfo.module = shader->shaderModule;
	stageCreateInfo.pName = "main";
	stageCreateInfo.pSpecializationInfo = nullptr;

	shader->shaderCreateInfo = stageCreateInfo;
}

void DestroyVulkanShader(SEVulkan* vulk, SEVulkanShader* shader)
{
	vkDestroyShaderModule(vulk->logicalDevice, shader->shaderModule, nullptr);
}