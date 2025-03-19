#ifndef LIGHT_SOURCE_H
#define LIGHT_SOURCE_H

#if !defined(HLSL_SHADER) && !defined(GLSL_SHADER)
#include "../../Math/SEMath_Header.h"
#endif

#define NUM_POINT_LIGHTS 1
#define NUM_DIRECTIONAL_LIGHTS 1
#define NUM_SPOTLIGHTS 1

#if defined(HLSL_SHADER)
#define vec4 float4
#define vec3 float3
#define mat4 float4x4
#endif

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

struct PixelDesc
{
    vec3 normal;
    vec3 lightDir;
    vec3 viewDir;
    vec3 halfwayDir;
    float distance;
};

#endif
