#pragma once

#include "../Math/SEMath_Header.h"
#include "../ThirdParty/stb_ds.h"

struct Vertex
{
	vec4 position;
	vec4 normal;
	vec4 tangent;
	vec2 texCoords;
};

struct Triangle
{
	const Vertex* vertices;
	uint32_t i0;
	uint32_t i1;
	uint32_t i2;
};

void CreateTriangle(Triangle* triangle, const Vertex* pVertices, uint32_t i0, uint32_t i1, uint32_t i2);
vec4 ComputeNormal(const Triangle* const triangle);
vec4 ComputeTangent(const Triangle* const triangle);