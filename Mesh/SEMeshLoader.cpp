#include <stdio.h>
#include <cmath>
#include <Windows.h>
#include "SEMesh.h"
#include "..\FileSystem\SEFileSystem.h"

#define STB_DS_IMPLEMENTATION


int32_t StringToInt32(char* str)
{
	//formula used value = value * base + digit

	uint32_t index = 0;
	bool negative = false;
	if (str[index] == '-')
	{
		negative = true;
		++index;
	}


	int32_t value = 0;
	while (str[index] != '\0')
	{
		value = value * 10 + (str[index] - '0');
		++index;
	}

	if (negative)
		value = -value;

	return value;
}

int64_t StringToInt64(char* str)
{
	//formula used value = value * base + digit

	uint32_t index = 0;
	bool negative = false;
	if (str[index] == '-')
	{
		negative = true;
		++index;
	}

	int64_t value = 0;
	while (str[index] != '\0')
	{
		value = value * 10 + (str[index] - '0');
		++index;
	}

	if (negative)
		value = -value;

	return value;
}

float StringToFloat(char* str)
{
	//formula used value = value * base + digit
	//keep track of the number of digits right of the decimal point, call it N.
	//divde value by base^N (value / base^N)

	uint32_t index = 0;
	bool negative = false;
	if (str[index] == '-')
	{
		negative = true;
		++index;
	}

	char* dot = strchr(str, '.');
	uint32_t dotIndex = dot - str;

	uint32_t n = 0;
	float value = 0.0f;
	while (str[index] != '\0')
	{
		if (str[index] != '.')
			value = value * 10.0f + (str[index] - '0');

		if (index > dotIndex)
			++n;

		++index;
	}

	value = value / pow(10, n);

	if (negative)
		value = -value;

	return value;
}

double StringToDouble(char* str)
{
	//formula used value = value * base + digit
	//keep track of the number of digits right of the decimal point, call it N.
	//divde value by base^N (value / base^N)

	uint32_t index = 0;
	bool negative = false;
	if (str[index] == '-')
	{
		negative = true;
		++index;
	}

	char* dot = strchr(str, '.');
	uint32_t dotIndex = dot - str;

	uint32_t n = 0;
	double value = 0.0;
	while (str[index] != '\0')
	{
		if (str[index] != '.')
			value = value * 10.0 + (str[index] - '0');

		if (index > dotIndex)
			++n;

		++index;
	}

	value = value / pow(10, n);

	if (negative)
		value = -value;

	return value;
}

struct OBJVertexData
{
	//stb_ds arrays
	vec3* v = nullptr;
	vec3* vn = nullptr;
	vec2* vt = nullptr;
	bool vnExist = false;
	bool vtExist = false;
};

struct OBJFace
{
	int32_t vIndex;
	int32_t vtIndex = -1;
	int32_t vnIndex = -1;
};

void OBJParseVertex(char* buf, OBJVertexData* vData)
{
	uint32_t index = 0;
	uint32_t numIterations = 3;

	++index;
	char ch = buf[index];
	if (ch == 'n' || ch == 't')
	{
		if (ch == 't')
			numIterations = 2;

		index = index + 2;
	}
	else //ch == ' '
	{
		index = index + 1;
	}

	double value[3]{};

	for (uint32_t i = 0; i < numIterations; ++i)
	{
		char valueStr[32]{};
		while (buf[index] != ' ' && buf[index] != '\n')
		{
			strncat_s(valueStr, 32, &buf[index], 1);
			++index;
		}
		value[i] = StringToDouble(valueStr);
		++index;
	}

	if (ch == 'n')
	{
		vec3 vn(value[0], value[1], value[2]);
		arrpush(vData->vn, vn);
	}
	else if (ch == 't')
	{
		vec2 vt(value[0], value[1]);
		arrpush(vData->vt, vt);
	}
	else //ch == ' '
	{
		vec3 v(value[0], value[1], value[2]);
		arrpush(vData->v, v);
	}
}

void OBJParseFace(char* buf, OBJFace** faces)
{
	uint32_t index = 2;
	char valueStr[16]{};

	while (buf[index] != '\n' && buf[index] != '\0')
	{
		OBJFace face{};
		//vertex index
		valueStr[0] = '\0';
		while (buf[index] != ' ' && buf[index] != '\n' && buf[index] != '/')
		{
			strncat_s(valueStr, 16, &buf[index], 1);
			++index;
		}
		face.vIndex = StringToInt32(valueStr);

		if (buf[index] == '/')
		{
			if (buf[index + 1] == '/')
			{
				//normal index
				index = index + 2;
				valueStr[0] = '\0';
				while (buf[index] != ' ' && buf[index] != '\n')
				{
					strncat_s(valueStr, 16, &buf[index], 1);
					++index;
				}
				face.vnIndex = StringToInt32(valueStr);
			}
			else
			{
				//texture index
				++index;
				valueStr[0] = '\0';
				while (buf[index] != ' ' && buf[index] != '\n' && buf[index] != '/')
				{
					strncat_s(valueStr, 16, &buf[index], 1);
					++index;
				}
				face.vtIndex = StringToInt32(valueStr);

				if (buf[index] == '/')
				{
					//normal index
					++index;
					valueStr[0] = '\0';
					while (buf[index] != ' ' && buf[index] != '\n')
					{
						strncat_s(valueStr, 16, &buf[index], 1);
						++index;
					}
					face.vnIndex = StringToInt32(valueStr);
				}
			}
		}
		++index;
		arrpush(*faces, face);
	}
}

void ParseOBJ(const char* filename, Vertex** vertices, uint32_t** indices, uint32_t* numVertices, uint32_t* numIndices)
{
	FILE* file = nullptr;

	fopen_s(&file, filename, "r");

	char errorMsg[256]{};
	if (!file)
	{
		strcat_s(errorMsg, "Failed to open file ");
		strcat_s(errorMsg, filename);
		strcat_s(errorMsg, ". Exiting prorgam.");
		MessageBoxA(nullptr, errorMsg, "File open error.", MB_OK);
		exit(-1);
	}

	OBJVertexData vData;
	OBJFace* faces = nullptr; //a stb_ds array

	char buf[128]{};
	bool firstFace = true;
	uint32_t numIndicesPerFace = 0;
	while (fgets(buf, 128, file) != nullptr)
	{
		uint32_t index = 0;
		if (buf[index] == 'v')
		{
			++index;
			if (buf[index] == 'n' || buf[index] == 't')
			{
				if (buf[index] == 'n')
					vData.vnExist = true;
				else if (buf[index] == 't')
					vData.vtExist = true;

				index = index + 2;
			}
			else
			{
				++index;
			}

			if (buf[index] == '-' || (buf[index] >= '0' && buf[index] <= '9'))
			{
				OBJParseVertex(buf, &vData);
			}
		}
		else if (buf[index] == 'f')
		{
			if (firstFace == true)
			{
				uint32_t i = 0;
				while (buf[i] != '\0')
				{
					if (buf[i] == ' ')
						++numIndicesPerFace;

					++i;
				}
				firstFace = false;
			}

			index = index + 2;
			if (buf[index] >= '0' && buf[index] <= '9')
			{
				OBJParseFace(buf, &faces);
			}
		}
	}

	Vertex* vertexList = (Vertex*)calloc(arrlenu(vData.v), sizeof(Vertex));
	for (uint32_t i = 0; i < arrlenu(faces); i = i + numIndicesPerFace)
	{
		for (uint32_t j = 0; j < numIndicesPerFace; ++j)
		{
			int32_t vIndex = faces[i + j].vIndex - 1;
			vec3 v = vData.v[vIndex];
			vertexList[vIndex].position = vec4(v.GetX(), v.GetY(), v.GetZ(), 1.0f);

			int32_t vnIndex = faces[i + j].vnIndex - 1;
			if (vnIndex > -1)
			{
				vec3 vn = vData.vn[vnIndex];
				vertexList[vnIndex].normal = vec4(vn.GetX(), vn.GetY(), vn.GetZ(), 0.0f);
			}

			int32_t vtIndex = faces[i + j].vtIndex - 1;
			if (vtIndex > -1)
				vertexList[vtIndex].texCoords = vData.vt[vtIndex];
		}

		if (numIndicesPerFace == 3)
		{
			int32_t vIndex = faces[i].vIndex - 1;
			arrpush(*indices, vIndex);

			vIndex = faces[i + 1].vIndex - 1;
			arrpush(*indices, vIndex);

			vIndex = faces[i + 2].vIndex - 1;
			arrpush(*indices, vIndex);
		}
		else if (numIndicesPerFace == 4)
		{
			//0 1 2
			//abc
			int32_t vIndex = faces[i].vIndex - 1;
			arrpush(*indices, vIndex);

			vIndex = faces[i + 1].vIndex - 1;
			arrpush(*indices, vIndex);

			vIndex = faces[i + 2].vIndex - 1;
			arrpush(*indices, vIndex);

			//0 2 3
			//acd
			vIndex = faces[i].vIndex - 1;
			arrpush(*indices, vIndex);

			vIndex = faces[i + 2].vIndex - 1;
			arrpush(*indices, vIndex);

			vIndex = faces[i + 3].vIndex - 1;
			arrpush(*indices, vIndex);
		}
	}

	for (uint32_t i = 0; i < arrlenu(faces); i = i + 3)
	{
		Triangle triangle{};
		CreateTriangle(&triangle, vertexList, faces[i].vIndex - 1, faces[i + 1].vIndex - 1, faces[i + 2].vIndex - 1);

		uint32_t i0 = triangle.i0;
		uint32_t i1 = triangle.i1;
		uint32_t i2 = triangle.i2;

		if (vData.vnExist == false)
		{
			vec4 normal;
			normal = ComputeNormal(&triangle);
			vertexList[i0].normal += normal;
			vertexList[i1].normal += normal;
			vertexList[i2].normal += normal;
		}

		vec4 tangent;
		tangent = ComputeTangent(&triangle);
		vertexList[i0].tangent += tangent;
		vertexList[i1].tangent += tangent;
		vertexList[i2].tangent += tangent;
	}

	for (uint32_t i = 0; i < arrlenu(vData.v); ++i)
	{
		if (vData.vnExist == false)
			vertexList[i].normal = Normalize(vertexList[i].normal);

		vertexList[i].tangent = Normalize(vertexList[i].tangent);
		arrpush(*vertices, vertexList[i]);
	}

	*numVertices = arrlenu(vData.v);
	*numIndices = arrlenu(faces);
	free(vertexList);
	arrfree(vData.v);
	arrfree(vData.vt);
	arrfree(vData.vn);
	arrfree(faces);
}