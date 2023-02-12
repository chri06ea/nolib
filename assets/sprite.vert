#version 430 core					

out vec2 _uv;
out vec4 _color;

struct Sprite
{
	vec2 pos;
	vec2 size;
	vec2 atlas_offset;
	float scale;
	bool full_screen;
	vec4 color;
};

layout(std430, binding = 0) buffer VertexData
{
    Sprite sprites[];
};

#define H 2

void main()														
{		
	int vertex_index = gl_VertexID % 4;
	int sprite_index = gl_VertexID / 4;
	
	Sprite s = sprites[sprite_index];

	vec2 uvs[4] = 
    {
		vec2(s.atlas_offset.x, s.atlas_offset.y),
		vec2(s.atlas_offset.x + s.size.x, s.atlas_offset.y),
		vec2(s.atlas_offset.x, s.atlas_offset.y + s.size.y),
		vec2(s.atlas_offset.x + s.size.x, s.atlas_offset.y + s.size.y),
    };
		
	_uv = uvs[vertex_index];
	
	_color = s.color;
	
	if(s.full_screen)
	{
		vec2 vertices[4] = 
		{
			vec2(-1, +1), // TL
			vec2(+1, +1), // TR
			vec2(-1, -1), // BL
			vec2(+1, -1), // BR
		};

		gl_Position = vec4(vertices[vertex_index], 0, 1);
	}
	else
	{
		float ndc_w = 2.0 / 800.0;
		float ndc_h = 2.0 / 600.0;
	
		float px0 = -1.0 + (s.pos.x * ndc_w);
		float py0 = +1.0 - (s.pos.y * ndc_h);
		float px1 = -1.0 + ((s.pos.x + s.size.x * float(s.scale)) * ndc_w);
		float py1 = +1.0 - ((s.pos.y + s.size.y * float(s.scale)) * ndc_h);
	
		vec2 vertices[4] = 
		{
			vec2(px0, py0), // TL
			vec2(px1, py0), // TR
			vec2(px0, py1), // BL
			vec2(px1, py1), // BR
		};

		gl_Position = vec4(vertices[vertex_index], 0, 1);
	}
};
