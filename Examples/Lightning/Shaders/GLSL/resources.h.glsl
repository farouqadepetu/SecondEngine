#include "../ShaderLibrary/GLSL/phong.h.glsl"
#include "../ShaderLibrary/GLSL/pbr.h.glsl"

#define MATERIAL 0
#define TEXTURE 1

layout(row_major, set = 1, binding = 0) uniform PerFrameUniformBuffer
{
    mat4 view;
    mat4 projection;
    vec4 cameraPos;
} perFrameBuffer;

layout(row_major, set = 1, binding = 1) uniform PerObjectUniformBuffer
{
    mat4 model;
    mat4 transposeInverseModel;
} perObjectBuffer;

layout(row_major, set = 1, binding = 2) uniform PointLightUniformBuffer
{
    PointLight pointLight;
} pointLightBuffer;

layout(row_major, set = 1, binding = 3) uniform DirectionalLightUniformBuffer
{
    DirectionalLight directionalLight;
} directionalLightBuffer;

layout(row_major, set = 1, binding = 4) uniform SpotlightUniformBuffer
{
    Spotlight spotlight;
} spotlightBuffer;

layout(set = 1, binding = 5) uniform PhongMaterialUniformBuffer
{
    PhongMaterial material;
} phongMaterialBuffer;

layout(set = 1, binding = 6) uniform PBRMaterialUniformBuffer
{
    PBRMaterial material;
} pbrMaterialBuffer;

layout(push_constant) uniform RootConstants
{
    uint currentMapping;
    uint numPointLights;
    uint numDirectionalLights;
    uint numSpotlights;
    float normalScale;
}constants;

layout(set = 0, binding = 0) uniform texture2D gBricksColor;
layout(set = 0, binding = 1) uniform texture2D gBricksAO;
layout(set = 0, binding = 2) uniform texture2D gBricksRoughness;
layout(set = 0, binding = 3) uniform texture2D gBricksNormal;
layout(set = 0, binding = 4) uniform textureCube gTextureCube;
layout(set = 0, binding = 5) uniform sampler gSampler;
