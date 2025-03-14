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
    vec4 position;
    vec4 color;
    mat4 model;
};

struct DirectionalLight
{
    vec4 direction;
    vec4 color;
    mat4 model;
};

struct Spotlight
{
    vec4 position;
    vec4 direction;
    vec4 color;
    mat4 model;
    float innerCutoff;
    float outerCutoff;
};

#endif