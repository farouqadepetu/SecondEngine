#pragma once

#include "../Math/SEMath_Header.h"
#include "../ThirdParty/stb_ds.h"

struct Vertex
{
	vec4 position;
	vec4 normal;
	vec2 texCoords;
};

//Creates the vertices for an unit equilateral triangle centered around the origin.
//Unit meaning base = 1 and height = 1.
//Stores the vertices in a stb_ds array.
void CreateEquilateralTriangle(Vertex** vertices);

//Creates the vertices for an unit right triangle centered around the origin.
//Unit meaning base = 1 and height = 1.
//Stores the vertices in a stb_ds array.
void CreateRightTriangle(Vertex** vertices);

//Creates the vertices for an unit quad centered around the origin.
//Unit meaning width = 1 and height = 1.
//Stores the vertices and indices in a stb_ds array.
void CreateQuad(Vertex** vertices, uint32_t** indices, uint32_t* outIndexCount);

//Creates the vertices for an unit circle centered around the origin.
//Unit meaning radius = 1.
//Stores the vertices and indices in a stb_ds array.
void CreateCircle(Vertex** vertices, uint32_t** indices, uint32_t* outIndexCount);

//Frees the stb_ds array
void DestroyShape(Vertex** vertices, uint32_t** indices);

struct Shape
{
	vec3 position;
	quat orientation;
	vec3 scale;
	mat4 model;
};