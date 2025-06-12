#version 460
#include "resources.h.glsl"

layout(location = 0) in vec2 outTexCoords;

layout(location = 0) out vec4 finalColor;

void main()
{
    float depth;
    if (constants.currentLightSource == DIRECTIONAL_LIGHT)
    {
        depth = texture(sampler2D(gShadowMap, gSampler), outTexCoords).r;
    }
    else if (constants.currentLightSource == POINT_LIGHT)
    {
        depth = texture(sampler2D(gShadowMapPL[constants.debugIndex], gSampler), outTexCoords).r;
        depth = depth * 2.0f - 1.0f; // Back to NDC 
        depth = (2.0f * 1.0f * 10.0f) / (10.0f + 1.0f - depth * (10.0f - 1.0f));
        depth = depth / 10.0f;
    }

    finalColor = vec4(depth, depth, depth, 1.0f);
}