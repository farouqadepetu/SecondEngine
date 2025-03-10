#ifndef LIGHT_SOURCE_H
#define LIGHT_SOURCE_H

#define POINT 0
#define DIRECTIONAL 1
#define SPOTLIGHT 2
#define ALL 3

#define NUM_POINT_LIGHTS 1
#define NUM_DIRECTIONAL_LIGHTS 1
#define NUM_SPOTLIGHTS 1

struct PointLight
{
    float4 position;
    float4 color;
    float4x4 model;
};

struct DirectionalLight
{
    float4 direction;
    float4 color;
    float4x4 model;
};

struct Spotlight
{
    float4 position;
    float4 direction;
    float4 color;
    float4x4 model;
    float innerCutoff;
    float outerCutoff;
};

#endif