#version 430 core					

layout(std430, binding = 0) buffer VertexData
{
    vec2 vertices[];
};

void main()														
{			
	gl_Position = vec4(vertices[gl_VertexID], 0, 1);
};
