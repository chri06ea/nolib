#version 430 core					

layout(std430, binding = 0) buffer VertexData
{
    vec2 vertices[];
};

out vec2 _tex_coords;

void main()														
{		
	float tx = 0;
	float ty = 0;
	float tw = 32;
	float th = 32;

	vec2 textureCoords2[4] = 
    {
		vec2(tx, ty + th),
		vec2(tx, ty),
		vec2(tx + tw, ty + th),
		vec2(tx + tw, ty),
    };

	vec2 tex_coords = textureCoords2[gl_VertexID];

	_tex_coords = tex_coords;

	gl_Position = vec4(vertices[gl_VertexID], 0, 1);
};
