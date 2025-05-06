#pragma once

#include "../Math/SEMath_Header.h"
#include "../ThirdParty/stb_ds.h"
#include "../Mesh/SEMesh.h"

//Create the vertices for an unit line.
//Unit meaning its length is one.
//Stores the vertices in a stb_ds array.
void CreateLine(Vertex** vertices);

//Creates the vertices for an unit equilateral triangle centered around the origin.
//Unit meaning base = 1 and height = 1.
//Stores the vertices in a stb_ds array.
void CreateEquilateralTriangle(Vertex** vertices, uint32_t** indices, uint32_t* outVertexCount, uint32_t* outIndexCount);

//Creates the vertices for an unit right triangle centered around the origin.
//Unit meaning base = 1 and height = 1.
//Stores the vertices in a stb_ds array.
void CreateRightTriangle(Vertex** vertices, uint32_t** indices, uint32_t* outVertexCount, uint32_t* outIndexCount);

//Creates the vertices and indices for an unit quad centered around the origin.
//Unit meaning width = 1 and height = 1.
//Stores the vertices and indices in a stb_ds array.
void CreateQuad(Vertex** vertices, uint32_t** indices, uint32_t* outVertexCount, uint32_t* outIndexCount);

//Creates the vertices and indices for an unit circle centered around the origin.
//Unit meaning radius = 1.
//Stores the vertices and indices in a stb_ds array.
void CreateCircle(Vertex** vertices, uint32_t** indices, uint32_t* outVertexCount, uint32_t* outIndexCount);

//Creates the vertices and indices for an unit box centered around the origin.
//Unit meaning width = 1, height = 1 and depth = 1.
//Stores the vertices and indices in a stb_ds array.
void CreateBox(Vertex** vertices, uint32_t** indices, uint32_t* outVertexCount, uint32_t* outIndexCount);

//Creates the vertices and indices for an unit square pyramid centered around the origin.
//Unit meaning base area = 1, height = 1.
//Stores the vertices and indices in a stb_ds array.
void CreateSquarePyramid(Vertex** vertices, uint32_t** indices, uint32_t* outVertexCount, uint32_t* outIndexCount);

//Creates the vertices and indices for a frustrum. 
//The origin is at the center of the small square.
//Stores the vertices and indices in a stb_ds array.
void CreateFrustrum(Vertex** vertices, uint32_t** indices, uint32_t* outVertexCount, uint32_t* outIndexCount);

//Creates the vertices and indices for an unit triangular pyramid centered around the origin.
//Unit meaning base area = 1, height = 1.
//Stores the vertices and indices in a stb_ds array.
void CreateTriangularPyramid(Vertex** vertices, uint32_t** indices, uint32_t* outVertexCount, uint32_t* outIndexCount);

//Creates the vertices and indices for an unit sphere centered around the origin.
//Unit meaning radius = 1.
//Stores the vertices and indices in a stb_ds array.
void CreateSphere(Vertex** vertices, uint32_t** indices, uint32_t* outVertexCount, uint32_t* outIndexCount);

//Creates the vertices and indices for an unit hemisphere centered around the origin.
//Unit meaning radius = 1.
//Stores the vertices and indices in a stb_ds array.
void CreateHemiSphere(Vertex** vertices, uint32_t** indices, uint32_t* outVertexCount, uint32_t* outIndexCount, bool base);

//Creates the vertices and indices for an unit cylinder centered around the origin.
//Unit meaning radius = 1 and height = 1.
//Stores the vertices and indices in a stb_ds array.
void CreateCylinder(Vertex** vertices, uint32_t** indices, uint32_t* outVertexCount, uint32_t* outIndexCount, 
	bool topBase, bool bottomBase);

//Creates the vertices and indices for an unit cone centered around the origin.
//Unit meaning radius = 1 and height = 1.
//Stores the vertices and indices in a stb_ds array.
void CreateCone(Vertex** vertices, uint32_t** indices, uint32_t* outVertexCount, uint32_t* outIndexCount, bool bottomBase);

//Creates the vertices and indices for a torus centered around the origin.
//Stores the vertices and indices in a stb_ds array.
void CreateTorus(Vertex** vertices, uint32_t** indices, uint32_t* outVertexCount, uint32_t* outIndexCount, 
	float outerRaidus, float innerRadius);

//Frees the stb_ds array
void DestroyShape(Vertex** vertices, uint32_t** indices);

struct Shape
{
	vec3 position;
	quat orientation;
	vec3 scale;
	mat4 model;
};

void UpdateModelMatrix(Shape* shape);