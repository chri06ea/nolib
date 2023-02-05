#version 430 core					

out vec2 _uv;
out vec4 _color;

struct Sprite
{
	vec2 pos;
	vec2 size;
	vec2 atlas_offset;
};

layout(std430, binding = 0) buffer VertexData
{
    Sprite sprites[];
};

void main()														
{		
	int vertex_index = gl_VertexID % 4;
	int sprite_index = gl_VertexID / 4;

	Sprite s = sprites[1];

	float tx = s.pos.x;
	float ty = s.pos.y;
	float tw = s.size.x;
	float th = s.size.x;

	vec2 uvs[4] = 
    {
		vec2(tx, ty),
		vec2(tx + tw, ty),
		vec2(tx, ty + th),
		vec2(tx + tw, ty + th),
    };

	vec2 vertices[4] = 
	{
		vec2(-0.8, +0.8), // TL
		vec2(+0.8, +0.8), // TR
		vec2(-0.8, -0.8), // BL
		vec2(+0.8, -0.8), // BR
	};

	_uv = uvs[vertex_index];
	
	_color = vec4(1,1,1,1);

	gl_Position = vec4(vertices[vertex_index], 0, 1);
};
