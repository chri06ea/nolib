#version 430 core					

layout(std430, binding = 0) buffer VertexData
{
    vec2 vertices[];
};

out vec2 _tex_coords;

void main()														
{		
	float textureCoords[6] = 
    {
		0, 0,
		1.0, 1.0,
		1.0, 1.0
    };

	vec2 textureCoords2[3] = 
    {
		vec2(0.0, 0.0),
		vec2(0.0, 10.0),
		vec2(10.0, 10.0)
    };

	vec2 tex_coords = textureCoords2[gl_VertexID];

	_tex_coords = tex_coords;

	gl_Position = vec4(vertices[gl_VertexID], 0, 1);
};
