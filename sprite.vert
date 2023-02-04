#version 330 core												
layout (location = 0) in vec2 a_pos;							
layout (location = 1) in vec4 a_color;							
layout (location = 2) in vec2 a_texture_coords;					
layout (location = 3) in vec2 a_texture_size;					
out vec4 a_color_;												
out vec2 a_texture_coords_;										
void main()														
{																
	a_color_ = a_color;											
	a_texture_coords_ = a_texture_coords;	 					
																
	gl_Position = vec4(a_pos, 0.0, 1.0);						
};
