#pragma once

#include <unordered_map>
#include <string>

#include "SEVulkan.h"
#include "SEFileSystem.h"


enum ShaderType
{
	VERTEX,
	PIXEL,
	COMPUTE
};

struct SEVulkanShader
{
	VkShaderModule shaderModule;
	VkPipelineShaderStageCreateInfo shaderCreateInfo;
};

void CreateVulkanShader(SEVulkan* vulk, const char* filename, ShaderType type, SEVulkanShader* shader);

void DestroyVulkanShader(SEVulkan* vulk, SEVulkanShader* shader);