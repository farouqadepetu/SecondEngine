#include "../ShaderLibrary/GLSL/pbr.h.glsl"

#define NUM_OBJECTS 7
#define NUM_LIGHT_DATA 8
#define DIRECTIONAL_LIGHT 0
#define POINT_LIGHT 1
#define SPOTLIGHT 2

layout(row_major, set = 1, binding = 0) uniform CameraUniformBuffer
{
    mat4 cameraView;
    mat4 cameraProjection;
    vec4 cameraPos;
} cameraBuffer;

layout(row_major, set = 1, binding = 1) uniform ObjectUniformBuffer
{
    mat4 objectModel[NUM_OBJECTS];
    mat4 objectInverseModel[NUM_OBJECTS];
    PBRMaterial material[NUM_OBJECTS];
} objectBuffer;

layout(row_major, set = 1, binding = 2) uniform LightSourceUniformBuffer
{
    mat4 lightSourceModel[NUM_LIGHT_DATA];
    mat4 lightSourceView[NUM_LIGHT_DATA];
    mat4 lightSourceProjection[NUM_LIGHT_DATA];
    vec4 lightPosition[NUM_LIGHT_DATA];
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
    float shadowBias;
    uint objectIndex;
    uint lightIndex;
    uint debugIndex;
}constants;

layout(set = 0, binding = 0) uniform texture2D gShadowMap;
layout(set = 0, binding = 1) uniform texture2D gShadowMapPL[6];
layout(set = 0, binding = 7) uniform sampler gSampler;
