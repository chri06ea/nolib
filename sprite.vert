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

	Sprite s = sprites[2];

	vec2 uvs[4] = 
    {
		vec2(s.atlas_offset.x, s.atlas_offset.y),
		vec2(s.atlas_offset.x + s.size.x, s.atlas_offset.y),
		vec2(s.atlas_offset.x, s.atlas_offset.y + s.size.y),
		vec2(s.atlas_offset.x + s.size.x, s.atlas_offset.y + s.size.y),
    };

	float ndc_w = 2.0 / 800;
	float ndc_h = 2.0 / 800;

	vec2 vertices[4] = 
	{
		vec2(-1.0 + (s.pos.x * ndc_w), 1.0 - (s.pos.y * ndc_h)), // TL
		vec2(-1.0 + ((s.pos.x + s.size.x) * ndc_w), 1.0 - (s.pos.y * ndc_h)), // TR
		vec2(-1.0 + (s.pos.x * ndc_w), 1.0 - ((s.pos.y + s.size.y) * ndc_h)), // BL
		vec2(-1.0 + ((s.pos.x + s.size.x) * ndc_w), 1.0 - ((s.pos.y + s.size.y) * ndc_h)), // BR
	};

	_uv = uvs[vertex_index];
	
	_color = vec4(1,1,1,1);

	gl_Position = vec4(vertices[vertex_index], 0, 1);
};
