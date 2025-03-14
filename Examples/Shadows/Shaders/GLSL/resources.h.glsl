#include "phong.h.glsl"
#include "pbr.h.glsl"

layout(row_major, set = 1, binding = 0) uniform CameraUniformBuffer
{
    mat4 cameraView;
    mat4 cameraProjection;
    vec4 cameraPos;
} cameraBuffer;

layout(row_major, set = 1, binding = 1) uniform ObjectUniformBuffer
{
    mat4 objectModel;
    mat4 objectInverseModel;
} objectBuffer;

layout(row_major, set = 1, binding = 2) uniform LightSourceUniformBuffer
{
    mat4 lightSourceModel;
    mat4 lightSourceView;
    mat4 lightSourceProjection;
} lightSourceBuffer;

layout(row_major, set = 1, binding = 3) uniform PointLightUniformBuffer
{
    PointLight pointLight;
} pointLightBuffer;

layout(row_major, set = 1, binding = 4) uniform DirectionalLightUniformBuffer
{
    DirectionalLight directionalLight;
} directionalLightBuffer;

layout(row_major, set = 1, binding = 5) uniform SpotlightUniformBuffer
{
    Spotlight spotlight;
} spotlightBuffer;

layout(push_constant) uniform RootConstants
{
    uint currentLightSource;
}constants;

layout(set = 0, binding = 0) uniform texture2D gBricksColor;
layout(set = 0, binding = 1) uniform texture2D gBricksAO;
layout(set = 0, binding = 2) uniform texture2D gBricksRoughness;
layout(set = 0, binding = 3) uniform texture2D gBricksNormal;
layout(set = 0, binding = 4) uniform texture2D gFloor;
layout(set = 0, binding = 5) uniform textureCube gTextureCube;
layout(set = 0, binding = 6) uniform sampler gSampler;
