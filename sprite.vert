#version 430 core					

layout(std430, binding = 0) buffer VertexData
{
    vec2 vertices[];
};

out vec2 _tex_coords;

void main()														
{		
	vec2 textureCoords2[4] = 
    {
		vec2(2.0, 2.0),
		vec2(2.0, 13.0),
		vec2(13.0, 13.0),
		vec2(13.0, 2.0),
    };

	vec2 tex_coords = textureCoords2[gl_VertexID];

	_tex_coords = tex_coords;

	gl_Position = vec4(vertices[gl_VertexID], 0, 1);
};
