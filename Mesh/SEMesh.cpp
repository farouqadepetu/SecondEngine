#include "SEMesh.h"

void CreateTriangle(Triangle* triangle, const Vertex* pVertices, uint32_t i0, uint32_t i1, uint32_t i2)
{
	triangle->vertices = pVertices;
	triangle->i0 = i0;
	triangle->i1 = i1;
	triangle->i2 = i2;
}

vec4 ComputeNormal(const Triangle* const triangle)
{
	uint32_t i0 = triangle->i0;
	uint32_t i1 = triangle->i1;
	uint32_t i2 = triangle->i2;

	vec4 p01(triangle->vertices[i1].position - triangle->vertices[i0].position);
	vec4 p02(triangle->vertices[i2].position - triangle->vertices[i0].position);

	vec3 a(p01.GetX(), p01.GetY(), p01.GetZ());
	vec3 b(p02.GetX(), p02.GetY(), p02.GetZ());

	vec3 result = Normalize(CrossProduct(a, b));

	return vec4(result.GetX(), result.GetY(), result.GetZ(), 0.0f);
}

vec4 ComputeTangent(const Triangle* const triangle)
{
	uint32_t i0 = triangle->i0;
	uint32_t i1 = triangle->i1;
	uint32_t i2 = triangle->i2;

	//e0 = p1 - p0
	//e1 = p2 - p0
	vec4 e0(triangle->vertices[i1].position - triangle->vertices[i0].position);
	vec4 e1(triangle->vertices[i2].position - triangle->vertices[i0].position);

	float e0x = e0.GetX();
	float e0y = e0.GetY();
	float e0z = e0.GetZ();

	float e1x = e1.GetX();
	float e1y = e1.GetY();
	float e1z = e1.GetZ();

	//u0 = t1x - t0x
	//u1 = t2x - t0x
	float u0 = triangle->vertices[i1].texCoords.GetX() - triangle->vertices[i0].texCoords.GetX();
	float u1 = triangle->vertices[i2].texCoords.GetX() - triangle->vertices[i0].texCoords.GetX();

	//v0 = t1y - t0y
	//v1 = t2y - t0y
	float v0 = triangle->vertices[i1].texCoords.GetY() - triangle->vertices[i0].texCoords.GetY();
	float v1 = triangle->vertices[i2].texCoords.GetY() - triangle->vertices[i0].texCoords.GetY();

	float f = (u0 * v1 - u1 * v0);

	if (f != 0)
		f = 1.0f / f;
	else
		f = 0.0f;

	float tx = f * (v1 * e0x - v0 * e1x);
	float ty = f * (v1 * e0y - v0 * e1y);
	float tz = f * (v1 * e0z - v0 * e1z);

	return vec4(tx, ty, tz, 0.0f);
}