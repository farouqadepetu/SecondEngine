#pragma once
#include <cstdint>

#include "SEMesh.h"

int32_t StringToInt32(char* str);
int64_t StringToInt64(char* str);
float StringToFloat(char* str);
double StringToDouble(char* str);

struct OBJVertexData
{
	//stb_ds arrays
	vec3* v = nullptr;
	vec3* vn = nullptr;
	vec2* vt = nullptr;
};

struct OBJFace
{
	int32_t vIndex;
	int32_t vtIndex = -1;
	int32_t vnIndex = -1;
};

void OBJParseVertex(char* buf, OBJVertexData* vData);
void OBJParseFace(char* buf, OBJFace** faces);
void ParseOBJ(const char* filename, Vertex** vertices, uint32_t** indices, uint32_t* numVertices, uint32_t* numIndices);