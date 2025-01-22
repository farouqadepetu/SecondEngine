#version 450

struct Vertex
{
	vec4 position;
	vec4 color;
	vec2 texCoords;
};

layout(binding = 0) buffer vertexBuffer
{
	Vertex vertices[8];
};

layout(binding = 1) buffer indexBuffer
{
	uint indices[36];
};

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1)

void main()
{
	vertices[0].position = vec4(-0.5f, 0.5f, -0.5f, 1.0f);
	vertices[0].color = vec4(0.0f, 0.0f, 1.0f, 1.0f);
	vertices[0].texCoords = vec2(0.0f, 0.0f);

	vertices[1].position = vec4(0.5f, 0.5f, -0.5f, 1.0f);
	vertices[1].color = vec4(0.0f, 0.0f, 1.0f, 1.0f);
	vertices[1].texCoords = vec2(1.0f, 0.0f);

	vertices[2].position = vec4(0.5f, -0.5f, -0.5f, 1.0f);
	vertices[2].color = vec4(0.0f, 0.0f, 1.0f, 1.0f);
	vertices[2].texCoords = vec2(1.0f, 1.0f);

	vertices[3].position = vec4(-0.5f, -0.5f, -0.5f, 1.0f);
	vertices[3].color = vec4(0.0f, 0.0f, 1.0f, 1.0f);
	vertices[3].texCoords = vec2(0.0f, 1.0f);

	vertices[4].position = vec4(0.5f, 0.5f, 0.5f, 1.0f);
	vertices[4].color = vec4(0.0f, 0.0f, 1.0f, 1.0f);
	vertices[4].texCoords = vec2(0.0f, 0.0f);

	vertices[5].position = vec4(0.5f, -0.5f, 0.5f, 1.0f);
	vertices[5].color = vec4(0.0f, 0.0f, 1.0f, 1.0f);
	vertices[5].texCoords = vec2(0.0f, 1.0f);

	vertices[6].position = vec4(-0.5f, -0.5f, 0.5f, 1.0f);
	vertices[6].color = vec4(0.0f, 0.0f, 1.0f, 1.0f);
	vertices[6].texCoords = vec2(1.0f, 1.0f);

	vertices[7].position = vec4(-0.5f, 0.5f, 0.5f, 1.0f);
	vertices[7].color = vec4(0.0f, 0.0f, 1.0f, 1.0f);
	vertices[7].texCoords = vec2(1.0f, 0.0f);

	//Front
	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
	indices[3] = 0;
	indices[4] = 2;
	indices[5] = 3;

	//Back
	indices[6] = 4;
	indices[7] = 7;
	indices[8] = 6;
	indices[9] = 4;
	indices[10] = 6;
	indices[11] = 5;

	//Top
	indices[12] = 7;
	indices[13] = 4;
	indices[14] = 1;
	indices[15] = 7;
	indices[16] = 1;
	indices[17] = 0;

	//Right
	indices[18] = 1;
	indices[19] = 4;
	indices[20] = 5;
	indices[21] = 1;
	indices[22] = 5;
	indices[23] = 2;

	//Left
	indices[24] = 7;
	indices[25] = 0;
	indices[26] = 3;
	indices[27] = 7;
	indices[28] = 3;
	indices[29] = 6;

	//Bottom
	indices[30] = 3;
	indices[31] = 2;
	indices[32] = 5;
	indices[33] = 3;
	indices[34] = 5;
	indices[35] = 6;
	
}
