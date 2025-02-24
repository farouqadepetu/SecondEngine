#include "SEShapes.h"
#include <cmath>

struct Triangle
{
	const Vertex* vertices;
	uint32_t i0;
	uint32_t i1;
	uint32_t i2;
};

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

void CreateEquilateralTriangle(Vertex** vertices, uint32_t** indices, uint32_t* outIndexCount)
{
	Vertex triangleVertices[3]{};

	triangleVertices[0].position.Set(-0.5f, -0.5f, 0.0f, 1.0f);
	triangleVertices[1].position.Set(0.0f, 0.5f, 0.0f, 1.0f);
	triangleVertices[2].position.Set(0.5f, -0.5f, 0.0f, 1.0f);

	triangleVertices[0].texCoords.Set(0.0f, 1.0f);
	triangleVertices[1].texCoords.Set(0.5f, 0.0f);
	triangleVertices[2].texCoords.Set(1.0f, 1.0f);

	Triangle triangle{};
	CreateTriangle(&triangle, triangleVertices, 0, 1, 2);
	vec4 normal = ComputeNormal(&triangle);
	triangleVertices[0].normal = normal;
	triangleVertices[1].normal = normal;
	triangleVertices[2].normal = normal;

	arrpush(*vertices, triangleVertices[0]);
	arrpush(*vertices, triangleVertices[1]);
	arrpush(*vertices, triangleVertices[2]);

	if (indices != nullptr)
	{
		arrpush(*indices, 0);
		arrpush(*indices, 1);
		arrpush(*indices, 2);

		if (outIndexCount != nullptr)
		{
			*outIndexCount = 3;
		}
	}
}


void CreateRightTriangle(Vertex** vertices, uint32_t** indices, uint32_t* outIndexCount)
{
	Vertex triangleVertices[3]{};

	triangleVertices[0].position.Set(-0.5f, -0.5f, 0.0f, 1.0f);
	triangleVertices[1].position.Set(-0.5f, 0.5f, 0.0f, 1.0f);
	triangleVertices[2].position.Set(0.5f, -0.5f, 0.0f, 1.0f);

	triangleVertices[0].texCoords.Set(0.0f, 1.0f);
	triangleVertices[1].texCoords.Set(0.5f, 0.0f);
	triangleVertices[2].texCoords.Set(1.0f, 1.0f);

	Triangle triangle{};
	CreateTriangle(&triangle, triangleVertices, 0, 1, 2);
	vec4 normal = ComputeNormal(&triangle);
	triangleVertices[0].normal = normal;
	triangleVertices[1].normal = normal;
	triangleVertices[2].normal = normal;

	arrpush(*vertices, triangleVertices[0]);
	arrpush(*vertices, triangleVertices[1]);
	arrpush(*vertices, triangleVertices[2]);

	if (indices != nullptr)
	{
		arrpush(*indices, 0);
		arrpush(*indices, 1);
		arrpush(*indices, 2);

		if (outIndexCount != nullptr)
		{
			*outIndexCount = 3;
		}
	}
}

void CreateQuad(Vertex** vertices, uint32_t** indices, uint32_t* outIndexCount)
{
	Vertex quadVertices[4]{};
	Triangle triangles[2]{};
	uint32_t indexCount = 0;

	quadVertices[0].position.Set(-0.5f, 0.5f, 0.0f, 1.0f);
	quadVertices[1].position.Set(0.5f, 0.5f, 0.0f, 1.0f);
	quadVertices[2].position.Set(0.5f, -0.5f, 0.0f, 1.0f);
	quadVertices[3].position.Set(-0.5f, -0.5f, 0.0f, 1.0f);

	quadVertices[0].texCoords.Set(0.0f, 0.0f);
	quadVertices[1].texCoords.Set(1.0f, 0.0f);
	quadVertices[2].texCoords.Set(1.0f, 1.0f);
	quadVertices[3].texCoords.Set(0.0f, 1.0f);

	CreateTriangle(&triangles[0], quadVertices, 0, 1, 2);
	CreateTriangle(&triangles[1], quadVertices, 0, 2, 3);
	for (uint32_t i = 0; i < 2; ++i)
	{
		vec4 normal = ComputeNormal(&triangles[i]);
		uint32_t i0 = triangles[i].i0;
		uint32_t i1 = triangles[i].i1;
		uint32_t i2 = triangles[i].i2;
		quadVertices[i0].normal += normal;
		quadVertices[i1].normal += normal;
		quadVertices[i2].normal += normal;

		arrpush(*indices, i0);
		arrpush(*indices, i1);
		arrpush(*indices, i2);

		indexCount += 3;
	}

	for (uint32_t i = 0; i < 4; ++i)
	{
		quadVertices[i].normal = Normalize(quadVertices[i].normal);
	}

	arrpush(*vertices, quadVertices[0]);
	arrpush(*vertices, quadVertices[1]);
	arrpush(*vertices, quadVertices[2]);
	arrpush(*vertices, quadVertices[3]);

	*outIndexCount = indexCount;
}

void CreateCircle(Vertex** vertices, uint32_t** indices, uint32_t* outIndexCount)
{
	//Parameteric equations used to the produce the vertices of a unit circle.
	//x = cos(angle)
	//y = sin(angle)

	Vertex* vertexList = nullptr;
	uint32_t indexCount = 0;

	float stepRate = 4.0f; //degrees
	float angle = 0.0f;

	//Center vertex
	Vertex vertex;
	vertex.position.Set(0.0f, 0.0f, 0.0f, 1.0f);
	vertex.texCoords.Set(0.5f, 0.5f);
	arrpush(vertexList, vertex);

	//Compute the vertices and texture coordinates.
	while (angle <= 360.0f)
	{
		float rad = angle * (PI / 180.0f);
		float x = cos(rad);
		float y = sin(rad);
		vertex.position.Set(x, y, 0.0f, 1.0f);

		//Changing range from [-1, 1] -> [1, 0]
		float u = (1.0f - x) * 0.5f;
		float v = (1.0f - y) * 0.5f;

		vertex.texCoords.Set(u, v);

		arrpush(vertexList, vertex);

		angle += stepRate;
	}

	//Create the triangles.
	//Using the triangle fan method, so the number of triangles equals to the number of vertices of the circle.
	Triangle* triangles = nullptr;
	uint32_t numTriangles = arrlenu(vertexList) - 2;
	for (uint32_t i = 0; i < numTriangles; ++i)
	{
		Triangle triangle{};
		CreateTriangle(&triangle, vertexList, 0, i + 2, i + 1);

		//Compute the normals
		vec4 normal = ComputeNormal(&triangle);
		uint32_t i0 = triangle.i0;
		uint32_t i1 = triangle.i1;
		uint32_t i2 = triangle.i2;
		vertexList[i0].normal += normal;
		vertexList[i1].normal += normal;
		vertexList[i2].normal += normal;

		arrpush(*indices, i0);
		arrpush(*indices, i1);
		arrpush(*indices, i2);
		arrpush(triangles, triangle);

		indexCount += 3;
	}

	for (uint32_t i = 0; i < arrlenu(vertexList); ++i)
	{
		vertexList[i].normal = Normalize(vertexList[i].normal);
		arrpush(*vertices, vertexList[i]);
	}

	*outIndexCount = indexCount;
	arrfree(triangles);
	arrfree(vertexList);
}

void CreateBox(Vertex** vertices, uint32_t** indices, uint32_t* outIndexCount)
{
	Vertex* vertexList = nullptr;
	Triangle* triangles = nullptr;
	uint32_t indexCount = 0;

	Vertex vertex;
	vertex.position = vec4(-0.5f, 0.5f, -0.5f, 1.0f);
	vertex.texCoords = vec2(0.0f, 0.0f);
	arrpush(vertexList, vertex);

	vertex.position = vec4(0.5f, 0.5f, -0.5f, 1.0f);
	vertex.texCoords = vec2(1.0f, 0.0f);
	arrpush(vertexList, vertex);

	vertex.position = vec4(0.5f, -0.5f, -0.5f, 1.0f);
	vertex.texCoords = vec2(1.0f, 1.0f);
	arrpush(vertexList, vertex);

	vertex.position = vec4(-0.5f, -0.5f, -0.5f, 1.0f);
	vertex.texCoords = vec2(0.0f, 1.0f);
	arrpush(vertexList, vertex);

	vertex.position = vec4(0.5f, 0.5f, 0.5f, 1.0f);
	vertex.texCoords = vec2(0.0f, 0.0f);
	arrpush(vertexList, vertex);

	vertex.position = vec4(0.5f, -0.5f, 0.5f, 1.0f);
	vertex.texCoords = vec2(0.0f, 1.0f);
	arrpush(vertexList, vertex);

	vertex.position = vec4(-0.5f, -0.5f, 0.5f, 1.0f);
	vertex.texCoords = vec2(1.0f, 1.0f);
	arrpush(vertexList, vertex);

	vertex.position = vec4(-0.5f, 0.5f, 0.5f, 1.0f);
	vertex.texCoords = vec2(1.0f, 0.0f);
	arrpush(vertexList, vertex);

	Triangle triangle{};

	//Front
	CreateTriangle(&triangle, vertexList, 0, 1, 2);
	arrpush(triangles, triangle);
	CreateTriangle(&triangle, vertexList, 0, 2, 3);
	arrpush(triangles, triangle);

	//Back
	CreateTriangle(&triangle, vertexList, 4, 7, 6);
	arrpush(triangles, triangle);
	CreateTriangle(&triangle, vertexList, 4, 6, 5);
	arrpush(triangles, triangle);

	//Top
	CreateTriangle(&triangle, vertexList, 7, 4, 1);
	arrpush(triangles, triangle);
	CreateTriangle(&triangle, vertexList, 7, 1, 0);
	arrpush(triangles, triangle);

	//Right
	CreateTriangle(&triangle, vertexList, 1, 4, 5);
	arrpush(triangles, triangle);
	CreateTriangle(&triangle, vertexList, 1, 5, 2);
	arrpush(triangles, triangle);

	//Left
	CreateTriangle(&triangle, vertexList, 7, 0, 3);
	arrpush(triangles, triangle);
	CreateTriangle(&triangle, vertexList, 7, 3, 6);
	arrpush(triangles, triangle);

	//Bottom
	CreateTriangle(&triangle, vertexList, 3, 2, 5);
	arrpush(triangles, triangle);
	CreateTriangle(&triangle, vertexList, 3, 5, 6);
	arrpush(triangles, triangle);

	for (uint32_t i = 0; i < 12; ++i)
	{
		//Compute the normals
		Triangle triangle = triangles[i];
		vec4 normal = ComputeNormal(&triangle);
		uint32_t i0 = triangle.i0;
		uint32_t i1 = triangle.i1;
		uint32_t i2 = triangle.i2;
		vertexList[i0].normal += normal;
		vertexList[i1].normal += normal;
		vertexList[i2].normal += normal;

		arrpush(*indices, i0);
		arrpush(*indices, i1);
		arrpush(*indices, i2);

		indexCount += 3;
	}

	for (uint32_t i = 0; i < arrlenu(vertexList); ++i)
	{
		vertexList[i].normal = Normalize(vertexList[i].normal);
		arrpush(*vertices, vertexList[i]);
	}

	*outIndexCount = indexCount;
	arrfree(triangles);
	arrfree(vertexList);
}

void CreateSquarePyramid(Vertex** vertices, uint32_t** indices, uint32_t* outIndexCount)
{
	Vertex* vertexList = nullptr;
	Triangle* triangles = nullptr;
	uint32_t indexCount = 0;

	Vertex vertex;

	//Base
	vertex.position = vec4(-0.5f, -0.5f, 0.5f, 1.0f);
	vertex.texCoords = vec2(0.0f, 0.0f);
	arrpush(vertexList, vertex);

	vertex.position = vec4(0.5f, -0.5f, 0.5f, 1.0f);
	vertex.texCoords = vec2(1.0f, 0.0f);
	arrpush(vertexList, vertex);

	vertex.position = vec4(0.5f, -0.5f, -0.5f, 1.0f);
	vertex.texCoords = vec2(1.0f, 1.0f);
	arrpush(vertexList, vertex);

	vertex.position = vec4(-0.5f, -0.5f, -0.5f, 1.0f);
	vertex.texCoords = vec2(0.0f, 1.0f);
	arrpush(vertexList, vertex);

	//Top Cneter Vertex
	vertex.position = vec4(0.0f, 0.5f, 0.0f, 1.0f);
	vertex.texCoords = vec2(0.5f, 0.0f);
	arrpush(vertexList, vertex);

	Triangle triangle{};

	//Base Triangles
	CreateTriangle(&triangle, vertexList, 2, 1, 0);
	arrpush(triangles, triangle);
	CreateTriangle(&triangle, vertexList, 2, 0, 3);
	arrpush(triangles, triangle);

	//Back
	CreateTriangle(&triangle, vertexList, 1, 4, 0);
	arrpush(triangles, triangle);

	//Right
	CreateTriangle(&triangle, vertexList, 2, 4, 1);
	arrpush(triangles, triangle);

	//Front
	CreateTriangle(&triangle, vertexList, 3, 4, 2);
	arrpush(triangles, triangle);

	//Left
	CreateTriangle(&triangle, vertexList, 0, 4, 3);
	arrpush(triangles, triangle);

	for (uint32_t i = 0; i < 6; ++i)
	{
		//Compute the normals
		Triangle triangle = triangles[i];
		vec4 normal = ComputeNormal(&triangle);
		uint32_t i0 = triangle.i0;
		uint32_t i1 = triangle.i1;
		uint32_t i2 = triangle.i2;
		vertexList[i0].normal += normal;
		vertexList[i1].normal += normal;
		vertexList[i2].normal += normal;

		arrpush(*indices, i0);
		arrpush(*indices, i1);
		arrpush(*indices, i2);

		indexCount += 3;
	}

	for (uint32_t i = 0; i < arrlenu(vertexList); ++i)
	{
		vertexList[i].normal = Normalize(vertexList[i].normal);
		arrpush(*vertices, vertexList[i]);
	}

	*outIndexCount = indexCount;
	arrfree(triangles);
	arrfree(vertexList);
}

void CreateTriangularPyramid(Vertex** vertices, uint32_t** indices, uint32_t* outIndexCount)
{
	Vertex* vertexList = nullptr;
	Triangle* triangles = nullptr;
	uint32_t indexCount = 0;

	Vertex vertex;

	//Base
	vertex.position = vec4(-0.5f, -0.5f, -0.5f, 1.0f);
	vertex.texCoords = vec2(0.0f, 1.0f);
	arrpush(vertexList, vertex);

	vertex.position = vec4(0.0f, -0.5f, 0.5f, 1.0f);
	vertex.texCoords = vec2(0.5f, 0.0f);
	arrpush(vertexList, vertex);

	vertex.position = vec4(0.5f, -0.5f, -0.5f, 1.0f);
	vertex.texCoords = vec2(1.0f, 1.0f);
	arrpush(vertexList, vertex);

	//Top Cneter Vertex
	vertex.position = vec4(0.0f, 0.5f, 0.0f, 1.0f);
	vertex.texCoords = vec2(0.5f, 0.0f);
	arrpush(vertexList, vertex);

	Triangle triangle{};

	//Base Triangles
	CreateTriangle(&triangle, vertexList, 2, 1, 0);
	arrpush(triangles, triangle);

	//Right
	CreateTriangle(&triangle, vertexList, 2, 3, 1);
	arrpush(triangles, triangle);

	//Front
	CreateTriangle(&triangle, vertexList, 0, 3, 2);
	arrpush(triangles, triangle);

	//Left
	CreateTriangle(&triangle, vertexList, 1, 3, 0);
	arrpush(triangles, triangle);

	for (uint32_t i = 0; i < 4; ++i)
	{
		//Compute the normals
		Triangle triangle = triangles[i];
		vec4 normal = ComputeNormal(&triangle);
		uint32_t i0 = triangle.i0;
		uint32_t i1 = triangle.i1;
		uint32_t i2 = triangle.i2;
		vertexList[i0].normal += normal;
		vertexList[i1].normal += normal;
		vertexList[i2].normal += normal;

		arrpush(*indices, i0);
		arrpush(*indices, i1);
		arrpush(*indices, i2);

		indexCount += 3;
	}

	for (uint32_t i = 0; i < arrlenu(vertexList); ++i)
	{
		vertexList[i].normal = Normalize(vertexList[i].normal);
		arrpush(*vertices, vertexList[i]);
	}

	*outIndexCount = indexCount;
	arrfree(triangles);
	arrfree(vertexList);
}

void CreateSphere(Vertex** vertices, uint32_t** indices, uint32_t* outIndexCount)
{
	//Parameteric equations used to the produce the vertices of a unit sphere.
	//x = sin(phi) * cos(theta);
	//y = cos(phi)
	//z = sin(phi) * sin(theta)

	Vertex* vertexList = nullptr;
	uint32_t indexCount = 0;
	Vertex vertex;

	float thetaStepRate = 10.0f; //degrees
	float phiStepRate = 9.0f; //degrees

	uint32_t numCircles = (180.0f / phiStepRate);
	uint32_t numVerticesPerCircle = (360.0f / thetaStepRate) + 1;

	float u = 0.0f;
	float v = 0.0f;
	float uStep = 1.0f / (numVerticesPerCircle - 1);
	float vStep = 1.0f / numCircles;

	//Top Vertex
	vertex.position.Set(0.0f, 1.0f, 0.0f, 1.0f);
	vertex.texCoords.Set(0.5f, 0.5f);
	vertex.normal.Set(0.0f, 1.0f, 0.0f, 0.0f);
	arrpush(vertexList, vertex);
	v += vStep;

	//Compute the vertices and texture coordinates.
	for (uint32_t i = 0; i < numCircles - 1 ; ++i)
	{
		for (uint32_t j = 0; j < numVerticesPerCircle; ++j)
		{
			float x = sin(v * PI) * cos(u * PI2);
			float y = cos(v * PI);
			float z = sin(v * PI) * sin(u * PI2);
			vertex.position.Set(x, y, z, 1.0f);

			vertex.texCoords.Set(u, v);
			vertex.normal.Set(x, y , z, 0.0f);

			arrpush(vertexList, vertex);

			u += uStep;
		}
		v += vStep;
		u = 0.0f;
	}

	//Bottom Vertex
	vertex.position.Set(0.0f, -1.0f, 0.0f, 1.0f);
	vertex.texCoords.Set(0.5f, 0.5f);
	vertex.normal.Set(0.0f, -1.0f, 0.0f, 0.0f);
	arrpush(vertexList, vertex);

	//Top Triangles
	for (uint32_t i = 0; i < numVerticesPerCircle - 1; ++i)
	{
		arrpush(*indices, 0);
		arrpush(*indices, i + 2);
		arrpush(*indices, i + 1);

		indexCount += 3;
	}

	for (uint32_t i = 0; i < numCircles - 2; ++i)
	{
		for (uint32_t j = 0; j < numVerticesPerCircle - 1; ++j)
		{
			//Indices of the vertices that make up a quad
			uint32_t topLeft = (i * numVerticesPerCircle) + j + 1;
			uint32_t topRight = (i * numVerticesPerCircle) + j + 2;
			uint32_t bottomLeft = ((i + 1) * numVerticesPerCircle) + j + 1;
			uint32_t bottomRight = ((i + 1) * numVerticesPerCircle) + j + 2;

			arrpush(*indices, topLeft);
			arrpush(*indices, topRight);
			arrpush(*indices, bottomRight);

			arrpush(*indices, topLeft);
			arrpush(*indices, bottomRight);
			arrpush(*indices, bottomLeft);

			indexCount += 6;
		}
	}

	//Bottom Triangles
	uint32_t index = arrlenu(vertexList) - 1; //index of the bottom vertex
	uint32_t index2 = index - numVerticesPerCircle; //index of the first vertex of the last circle
	for (uint32_t i = 0; i < numVerticesPerCircle - 1; ++i)
	{
		arrpush(*indices, index);
		arrpush(*indices, index2 + i);
		arrpush(*indices, index2 + i + 1);

		indexCount += 3;
	}

	uint32_t numVertices = arrlenu(vertexList);
	for (uint32_t i = 0; i < numVertices; ++i)
	{
		vertexList[i].normal = Normalize(vertexList[i].normal);
		arrpush(*vertices, vertexList[i]);
	}

	*outIndexCount = indexCount;
	arrfree(vertexList);
}

void CreateHemiSphere(Vertex** vertices, uint32_t** indices, uint32_t* outIndexCount, bool base)
{
	//Parameteric equations used to the produce the vertices of a unit hemisphere.
	//x = sin(phi) * cos(theta);
	//y = cos(phi)
	//z = sin(phi) * sin(theta)

	Vertex* vertexList = nullptr;
	uint32_t indexCount = 0;
	Vertex vertex;

	float thetaStepRate = 10.0f; //degrees
	float phiStepRate = 9.0f; //degrees

	uint32_t numCircles = (90.0f / phiStepRate);
	uint32_t numVerticesPerCircle = (360.0f / thetaStepRate) + 1;

	float u = 0.0f;
	float v = 0.0f;
	float uStep = 1.0f / (numVerticesPerCircle - 1);
	float vStep = 1.0f / numCircles;

	//Top Vertex
	vertex.position.Set(0.0f, 0.5f, 0.0f, 1.0f);
	vertex.texCoords.Set(0.5f, 0.5f);
	vertex.normal.Set(0.0f, 0.5f, 0.0f, 0.0f);
	arrpush(vertexList, vertex);
	v += vStep;

	//Compute the vertices and texture coordinates.
	for (uint32_t i = 0; i < numCircles - 1; ++i)
	{
		for (uint32_t j = 0; j < numVerticesPerCircle; ++j)
		{
			float x = sin(v * PI / 2) * cos(u * PI2);
			float y = cos(v * PI / 2) - 0.5f;
			float z = sin(v * PI / 2) * sin(u * PI2);
			vertex.position.Set(x, y, z, 1.0f);

			vertex.texCoords.Set(u, v);
			vertex.normal.Set(x, y, z, 0.0f);

			arrpush(vertexList, vertex);

			u += uStep;
		}
		v += vStep;
		u = 0.0f;
	}

	//Top Triangles
	for (uint32_t i = 0; i < numVerticesPerCircle - 1; ++i)
	{
		arrpush(*indices, 0);
		arrpush(*indices, i + 2);
		arrpush(*indices, i + 1);

		indexCount += 3;
	}

	for (uint32_t i = 0; i < numCircles - 2; ++i)
	{
		for (uint32_t j = 0; j < numVerticesPerCircle - 1; ++j)
		{
			//Indices of the vertices that make up a quad
			uint32_t topLeft = (i * numVerticesPerCircle) + j + 1;
			uint32_t topRight = (i * numVerticesPerCircle) + j + 2;
			uint32_t bottomLeft = ((i + 1) * numVerticesPerCircle) + j + 1;
			uint32_t bottomRight = ((i + 1) * numVerticesPerCircle) + j + 2;

			arrpush(*indices, topLeft);
			arrpush(*indices, topRight);
			arrpush(*indices, bottomRight);

			arrpush(*indices, topLeft);
			arrpush(*indices, bottomRight);
			arrpush(*indices, bottomLeft);

			indexCount += 6;
		}
	}

	if (base == true)
	{
		//Center vertex for the base
		vertex.position.Set(0.0f, -0.25f, 0.0f, 1.0f);
		vertex.texCoords.Set(0.5f, 0.5f);
		vertex.normal.Set(0.0f, 0.25f, 0.0f, 0.0f);
		arrpush(vertexList, vertex);

		uint32_t index = arrlenu(vertexList) - 1; //index of the last vertex
		uint32_t index2 = index - numVerticesPerCircle; //index of the first vertex of the last circle
		for (uint32_t i = 0; i < numVerticesPerCircle - 1; ++i)
		{
			uint32_t i0 = index;
			uint32_t i1 = index2 + i;
			uint32_t i2 = index2 + i + 1;

			arrpush(*indices, index);
			arrpush(*indices, index2 + i);
			arrpush(*indices, index2 + i + 1);

			vertexList[i0].normal += vertexList[i1].normal;
			vertexList[i0].normal += vertexList[i2].normal;


			indexCount += 3;
		}
	}

	uint32_t numVertices = arrlenu(vertexList);
	for (uint32_t i = 0; i < numVertices; ++i)
	{
		vertexList[i].normal = Normalize(vertexList[i].normal);
		arrpush(*vertices, vertexList[i]);
	}

	*outIndexCount = indexCount;
	arrfree(vertexList);
}

void CreateCylinder(Vertex** vertices, uint32_t** indices, uint32_t* outIndexCount, bool topBase, bool bottomBase)
{
	//Parameteric equations used to the produce the vertices of a unit cylinder.
	//x = cos(theta);
	//y = h
	//z = sin(theta)

	Vertex* vertexList = nullptr;
	Triangle* triangleList = nullptr;
	uint32_t indexCount = 0;
	Vertex vertex;

	float thetaStepRate = 15.0f; //degrees
	float h = 1.0f;

	uint32_t numCircles = 4;
	uint32_t numVerticesPerCircle = (360.0f / thetaStepRate) + 1;

	float u = 0.0f;
	float v = 0.0f;
	float uStep = 1.0f / (numVerticesPerCircle - 1);
	float vStep = 1.0f / (numCircles - 1);

	//Compute the vertices and texture coordinates.
	for (uint32_t i = 0; i < numCircles; ++i)
	{
		for (uint32_t j = 0; j < numVerticesPerCircle; ++j)
		{
			float x = cos(u * PI2);
			float y = 0.5f - (v * h);
			float z =  sin(u * PI2);
			vertex.position.Set(x, y, z, 1.0f);

			vertex.texCoords.Set(u, v);

			arrpush(vertexList, vertex);

			u += uStep;
		}
		v += vStep;
		u = 0.0f;
	}

	//Generate the triangles
	for (uint32_t i = 0; i < numCircles - 1; ++i)
	{
		for (uint32_t j = 0; j < numVerticesPerCircle - 1; ++j)
		{
			//Indices of the vertices that make up a quad
			uint32_t topLeft = (i * numVerticesPerCircle) + j;
			uint32_t topRight = (i * numVerticesPerCircle) + j + 1;
			uint32_t bottomLeft = ((i + 1) * numVerticesPerCircle) + j;
			uint32_t bottomRight = ((i + 1) * numVerticesPerCircle) + j + 1;

			Triangle triangle{};
			CreateTriangle(&triangle, vertexList, topLeft, topRight, bottomRight);
			arrpush(triangleList, triangle);
			CreateTriangle(&triangle, vertexList, topLeft, bottomRight, bottomLeft);
			arrpush(triangleList, triangle);

			arrpush(*indices, topLeft);
			arrpush(*indices, topRight);
			arrpush(*indices, bottomRight);

			arrpush(*indices, topLeft);
			arrpush(*indices, bottomRight);
			arrpush(*indices, bottomLeft);

			indexCount += 6;
		}
	}

	if (topBase)
	{
		//Top Center Vertex
		vertex.position.Set(0.0f, 0.5f, 0.0f, 1.0f);
		vertex.texCoords.Set(0.5f, 0.5f);
		vertex.normal.Set(0.0f, 1.0f, 0.0f, 0.0f);
		arrpush(vertexList, vertex);
		uint32_t topIndex = arrlenu(vertexList) - 1;

		uint32_t index = (numCircles - 1) * numVerticesPerCircle; //index of the first vertex of the last circle
		for (uint32_t i = 0; i < numVerticesPerCircle - 1; ++i)
		{
			Triangle triangle{};
			CreateTriangle(&triangle, vertexList, topIndex, i + 1, i);
			arrpush(triangleList, triangle);
			arrpush(*indices, topIndex);
			arrpush(*indices, triangle.i1);
			arrpush(*indices, triangle.i2);

			indexCount += 3;
		}
	}

	if (bottomBase)
	{
		//Bottom Center Vertex
		vertex.position.Set(0.0f, -0.5f, 0.0f, 1.0f);
		vertex.texCoords.Set(0.5f, 0.5f);
		vertex.normal.Set(0.0f, -1.0f, 0.0f, 0.0f);
		arrpush(vertexList, vertex);
		uint32_t bottomIndex = arrlenu(vertexList) - 1;

		uint32_t index = (numCircles - 1) * numVerticesPerCircle; //index of the first vertex of the last circle
		for (uint32_t i = 0; i < numVerticesPerCircle - 1; ++i)
		{
			Triangle triangle{};
			CreateTriangle(&triangle, vertexList, bottomIndex, index + i, index + i + 1);
			arrpush(triangleList, triangle);
			arrpush(*indices, bottomIndex);
			arrpush(*indices, triangle.i1);
			arrpush(*indices, triangle.i2);

			indexCount += 3;
		}
	}

	//Compute the normals
	uint32_t numTriangles = arrlenu(triangleList);
	for (uint32_t i = 0; i < numTriangles; ++i)
	{
		vec4 normal = ComputeNormal(&triangleList[i]);

		uint32_t i0 = triangleList[i].i0;
		uint32_t i1 = triangleList[i].i1;
		uint32_t i2 = triangleList[i].i2;

		vertexList[i0].normal += normal;
		vertexList[i1].normal += normal;
		vertexList[i2].normal += normal;
	}

	uint32_t numVertices = arrlenu(vertexList);
	for (uint32_t i = 0; i < numVertices; ++i)
	{
		vertexList[i].normal = Normalize(vertexList[i].normal);
		arrpush(*vertices, vertexList[i]);
	}

	*outIndexCount = indexCount;
	arrfree(triangleList);
	arrfree(vertexList);
}

void CreateCone(Vertex** vertices, uint32_t** indices, uint32_t* outIndexCount, bool bottomBase)
{
	//Parameteric equations used to the produce the vertices of a unit cylinder.
	//x = rcos(theta);
	//y = r
	//z = rsin(theta)

	Vertex* vertexList = nullptr;
	Triangle* triangleList = nullptr;
	uint32_t indexCount = 0;
	Vertex vertex;

	float thetaStepRate = 15.0f; //degrees
	float r = 1.0f;

	uint32_t numCircles = 4;
	uint32_t numVerticesPerCircle = (360.0f / thetaStepRate) + 1;

	float u = 0.0f;
	float v = 0.0f;
	float uStep = 1.0f / (numVerticesPerCircle - 1);
	float vStep = 1.0f / (numCircles - 1);

	//Top Vertex
	vertex.position.Set(0.0f, 0.5f, 0.0f, 1.0f);
	vertex.texCoords.Set(0.5f, 0.5f);
	vertex.normal.Set(0.0f, 0.5f, 0.0f, 0.0f);
	arrpush(vertexList, vertex);
	v += vStep;

	//Compute the vertices and texture coordinates.
	for (uint32_t i = 0; i < numCircles - 1; ++i)
	{
		for (uint32_t j = 0; j < numVerticesPerCircle; ++j)
		{
			float x = (v * r) * cos(u * PI2);
			float y = 0.5f - (v * r);
			float z = (v * r) * sin(u * PI2);
			vertex.position.Set(x, y, z, 1.0f);

			vertex.texCoords.Set(u, v);

			arrpush(vertexList, vertex);

			u += uStep;
		}
		v += vStep;
		u = 0.0f;
	}

	//Top Triangles
	for (uint32_t i = 0; i < numVerticesPerCircle - 1; ++i)
	{
		arrpush(*indices, 0);
		arrpush(*indices, i + 2);
		arrpush(*indices, i + 1);

		indexCount += 3;
	}

	//Generate the triangles
	for (uint32_t i = 0; i < numCircles - 2; ++i)
	{
		for (uint32_t j = 0; j < numVerticesPerCircle - 1; ++j)
		{
			//Indices of the vertices that make up a quad
			uint32_t topLeft = (i * numVerticesPerCircle) + j + 1;
			uint32_t topRight = (i * numVerticesPerCircle) + j + 2;
			uint32_t bottomLeft = ((i + 1) * numVerticesPerCircle) + j + 1;
			uint32_t bottomRight = ((i + 1) * numVerticesPerCircle) + j + 2;

			Triangle triangle{};
			CreateTriangle(&triangle, vertexList, topLeft, topRight, bottomRight);
			arrpush(triangleList, triangle);
			CreateTriangle(&triangle, vertexList, topLeft, bottomRight, bottomLeft);
			arrpush(triangleList, triangle);

			arrpush(*indices, topLeft);
			arrpush(*indices, topRight);
			arrpush(*indices, bottomRight);

			arrpush(*indices, topLeft);
			arrpush(*indices, bottomRight);
			arrpush(*indices, bottomLeft);

			indexCount += 6;
		}
	}

	if (bottomBase)
	{
		//Bottom Center Vertex
		vertex.position.Set(0.0f, -0.5f, 0.0f, 1.0f);
		vertex.texCoords.Set(0.5f, 0.5f);
		vertex.normal.Set(0.0f, -1.0f, 0.0f, 0.0f);
		arrpush(vertexList, vertex);
		uint32_t bottomIndex = arrlenu(vertexList) - 1;

		uint32_t index = bottomIndex - numVerticesPerCircle; //index of the first vertex of the last circle
		for (uint32_t i = 0; i < numVerticesPerCircle - 1; ++i)
		{
			Triangle triangle{};
			CreateTriangle(&triangle, vertexList, bottomIndex, index + i, index + i + 1);
			arrpush(triangleList, triangle);
			arrpush(*indices, bottomIndex);
			arrpush(*indices, triangle.i1);
			arrpush(*indices, triangle.i2);

			indexCount += 3;
		}
	}

	//Compute the normals
	uint32_t numTriangles = arrlenu(triangleList);
	for (uint32_t i = 0; i < numTriangles; ++i)
	{
		vec4 normal = ComputeNormal(&triangleList[i]);

		uint32_t i0 = triangleList[i].i0;
		uint32_t i1 = triangleList[i].i1;
		uint32_t i2 = triangleList[i].i2;

		vertexList[i0].normal += normal;
		vertexList[i1].normal += normal;
		vertexList[i2].normal += normal;
	}

	uint32_t numVertices = arrlenu(vertexList);
	for (uint32_t i = 0; i < numVertices; ++i)
	{
		vertexList[i].normal = Normalize(vertexList[i].normal);
		arrpush(*vertices, vertexList[i]);
	}

	*outIndexCount = indexCount;
	arrfree(triangleList);
	arrfree(vertexList);
}

void CreateTorus(Vertex** vertices, uint32_t** indices, uint32_t* outIndexCount, float outerRaidus, float innerRadius)
{
	//Parameteric equations used to the produce the vertices of a unit cylinder.
	//x = (R + rcos(theta)) * cos(phi);
	//y = rsin(theta)
	//z = (R + rsin(theta)) * sin(phi)

	Vertex* vertexList = nullptr;
	Triangle* triangleList = nullptr;
	uint32_t indexCount = 0;
	Vertex vertex;

	float thetaStepRate = 9.0f; //degrees
	float phiStepRate = 9.0f;
	float r = 1.0f;

	uint32_t numCircles = (360.0f / phiStepRate) + 1;
	uint32_t numVerticesPerCircle = (360.0f / thetaStepRate) + 1;

	float u = 0.0f;
	float v = 0.0f;
	float uStep = 1.0f / (numVerticesPerCircle - 1);
	float vStep = 1.0f / (numCircles - 1);

	//Compute the vertices and texture coordinates.
	for (uint32_t i = 0; i < numCircles; ++i)
	{
		for (uint32_t j = 0; j < numVerticesPerCircle; ++j)
		{
			float x = (outerRaidus + innerRadius * cos(u * PI2)) * cos(v * PI2);
			float y = sin(u * PI2);
			float z = (outerRaidus + innerRadius * cos(u * PI2)) * sin(v * PI2);
			vertex.position.Set(x, y, z, 1.0f);

			vertex.texCoords.Set(u, v);

			arrpush(vertexList, vertex);

			u += uStep;
		}
		v += vStep;
		u = 0.0f;
	}

	//Generate the triangles
	for (uint32_t i = 0; i < numCircles - 1; ++i)
	{
		for (uint32_t j = 0; j < numVerticesPerCircle - 1; ++j)
		{
			//Indices of the vertices that make up a quad
			uint32_t topLeft = (i * numVerticesPerCircle) + j;
			uint32_t topRight = (i * numVerticesPerCircle) + j + 1;
			uint32_t bottomLeft = ((i + 1) * numVerticesPerCircle) + j;
			uint32_t bottomRight = ((i + 1) * numVerticesPerCircle) + j + 1;

			Triangle triangle{};
			CreateTriangle(&triangle, vertexList, topLeft, topRight, bottomRight);
			arrpush(triangleList, triangle);
			CreateTriangle(&triangle, vertexList, topLeft, bottomRight, bottomLeft);
			arrpush(triangleList, triangle);

			arrpush(*indices, topLeft);
			arrpush(*indices, topRight);
			arrpush(*indices, bottomRight);

			arrpush(*indices, topLeft);
			arrpush(*indices, bottomRight);
			arrpush(*indices, bottomLeft);

			indexCount += 6;
		}
	}

	//Compute the normals
	uint32_t numTriangles = arrlenu(triangleList);
	for (uint32_t i = 0; i < numTriangles; ++i)
	{
		vec4 normal = ComputeNormal(&triangleList[i]);

		uint32_t i0 = triangleList[i].i0;
		uint32_t i1 = triangleList[i].i1;
		uint32_t i2 = triangleList[i].i2;

		vertexList[i0].normal += normal;
		vertexList[i1].normal += normal;
		vertexList[i2].normal += normal;
	}

	uint32_t numVertices = arrlenu(vertexList);
	for (uint32_t i = 0; i < numVertices; ++i)
	{
		vertexList[i].normal = Normalize(vertexList[i].normal);
		arrpush(*vertices, vertexList[i]);
	}

	*outIndexCount = indexCount;
	arrfree(triangleList);
	arrfree(vertexList);

}

void DestroyShape(Vertex** vertices, uint32_t** indices)
{
	arrfree(*indices);
	arrfree(*vertices);
}

void UpdateModelMatrix(Shape* shape)
{
	shape->model = mat4::Scale(shape->scale.GetX(), shape->scale.GetY(), shape->scale.GetZ()) *
		QuaternionToMatrix(shape->orientation) * mat4::Translate(shape->position.GetX(), shape->position.GetY(), shape->position.GetZ());
}

