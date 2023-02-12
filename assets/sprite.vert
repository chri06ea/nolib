#version 430 core					

out vec2 _uv;
out vec4 _color;

struct Sprite
{
	vec2 screen_pos;
	vec2 screen_size;
	vec2 sprite_atlas_pos;
	vec2 sprite_size;
	vec4 color;

	float scale;
	bool full_screen;
	
	// TODO: Find out why this padding is needed
	bool ff2;
	float ff3;
};

layout(std140, binding = 0) buffer VertexData
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
		vec2(s.sprite_atlas_pos.x, s.sprite_atlas_pos.y),
		vec2(s.sprite_atlas_pos.x + s.sprite_size.x, s.sprite_atlas_pos.y),
		vec2(s.sprite_atlas_pos.x, s.sprite_atlas_pos.y + s.sprite_size.y),
		vec2(s.sprite_atlas_pos.x + s.sprite_size.x, s.sprite_atlas_pos.y + s.sprite_size.y),
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
	
		float px0 = -1.0 + (s.screen_pos.x * ndc_w);
		float py0 = +1.0 - (s.screen_pos.y * ndc_h);
		float px1 = -1.0 + ((s.screen_pos.x + s.screen_size.x * s.scale) * ndc_w);
		float py1 = +1.0 - ((s.screen_pos.y + s.screen_size.y * s.scale) * ndc_h);
	
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
