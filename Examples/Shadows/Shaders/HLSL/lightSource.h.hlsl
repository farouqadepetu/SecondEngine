#ifndef LIGHT_SOURCE_H
#define LIGHT_SOURCE_H

#define DIRECTIONAL 0
#define POINT 1
#define SPOTLIGHT 2
#define ALL 3

#define NUM_POINT_LIGHTS 1
#define NUM_DIRECTIONAL_LIGHTS 1
#define NUM_SPOTLIGHTS 1

struct PointLight
{
    float4 position;
    float4 color;
};

struct DirectionalLight
{
    float4 direction;
    float4 color;
};

struct Spotlight
{
    float4 position;
    float4 direction;
    float4 color;
    float innerCutoff;
    float outerCutoff;
};

#endif