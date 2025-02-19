#include "SEShapes.h"

struct TriangleIndices
{
	const Vertex* vertices;
	uint32_t i0;
	uint32_t i1;
	uint32_t i2;
};

void CreateTriangleIndices(TriangleIndices* triangle, const Vertex* pVertices, uint32_t i0, uint32_t i1, uint32_t i2)
{
	triangle->vertices = pVertices;
	triangle->i0 = i0;
	triangle->i1 = i1;
	triangle->i2 = i2;
}

vec4 ComputeNormal(const TriangleIndices* const triangle)
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

void CreateEquilateralTriangle(Vertex** vertices)
{
	Vertex triangleVertices[3]{};

	triangleVertices[0].position.Set(-0.5f, -0.5f, 0.0f, 1.0f);
	triangleVertices[1].position.Set(0.0f, 0.5f, 0.0f, 1.0f);
	triangleVertices[2].position.Set(0.5f, -0.5f, 0.0f, 1.0f);

	triangleVertices[0].texCoords.Set(0.0f, 1.0f);
	triangleVertices[1].texCoords.Set(0.5f, 0.0f);
	triangleVertices[2].texCoords.Set(1.0f, 1.0f);

	TriangleIndices triangle{};
	CreateTriangleIndices(&triangle, triangleVertices, 0, 1, 2);
	vec4 normal = ComputeNormal(&triangle);
	triangleVertices[0].normal = normal;
	triangleVertices[1].normal = normal;
	triangleVertices[2].normal = normal;

	arrpush(*vertices, triangleVertices[0]);
	arrpush(*vertices, triangleVertices[1]);
	arrpush(*vertices, triangleVertices[2]);
}


void CreateRightTriangle(Vertex** vertices)
{
	Vertex triangleVertices[3]{};

	triangleVertices[0].position.Set(-0.5f, -0.5f, 0.0f, 1.0f);
	triangleVertices[1].position.Set(-0.5f, 0.5f, 0.0f, 1.0f);
	triangleVertices[2].position.Set(0.5f, -0.5f, 0.0f, 1.0f);

	triangleVertices[0].texCoords.Set(0.0f, 1.0f);
	triangleVertices[1].texCoords.Set(0.5f, 0.0f);
	triangleVertices[2].texCoords.Set(1.0f, 1.0f);

	TriangleIndices triangle{};
	CreateTriangleIndices(&triangle, triangleVertices, 0, 1, 2);
	vec4 normal = ComputeNormal(&triangle);
	triangleVertices[0].normal = normal;
	triangleVertices[1].normal = normal;
	triangleVertices[2].normal = normal;

	arrpush(*vertices, triangleVertices[0]);
	arrpush(*vertices, triangleVertices[1]);
	arrpush(*vertices, triangleVertices[2]);
}

void CreateQuad(Vertex** vertices, uint32_t** indices, uint32_t* outIndexCount)
{
	Vertex quadVertices[4]{};
	TriangleIndices triangles[2]{};
	uint32_t indexCount = 0;

	quadVertices[0].position.Set(-0.5f, 0.5f, 0.0f, 1.0f);
	quadVertices[1].position.Set(0.5f, 0.5f, 0.0f, 1.0f);
	quadVertices[2].position.Set(0.5f, -0.5f, 0.0f, 1.0f);
	quadVertices[3].position.Set(-0.5f, -0.5f, 0.0f, 1.0f);

	quadVertices[0].texCoords.Set(0.0f, 0.0f);
	quadVertices[1].texCoords.Set(1.0f, 0.0f);
	quadVertices[2].texCoords.Set(1.0f, 1.0f);
	quadVertices[3].texCoords.Set(0.0f, 1.0f);

	CreateTriangleIndices(&triangles[0], quadVertices, 0, 1, 2);
	CreateTriangleIndices(&triangles[1], quadVertices, 0, 2, 3);
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
	TriangleIndices* triangles = nullptr;
	uint32_t numTriangles = arrlenu(vertexList) - 2;
	for (uint32_t i = 0; i < numTriangles; ++i)
	{
		TriangleIndices triangle{};
		CreateTriangleIndices(&triangle, vertexList, 0, i + 2, i + 1);

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
	arrfree(vertexList);
	arrfree(triangles);
}

void DestroyShape(Vertex** vertices, uint32_t** indices)
{
	arrfree(*vertices);
	arrfree(*indices);
}

