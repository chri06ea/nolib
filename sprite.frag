#version 330 core												
in vec4 a_color_;												
in vec2 a_texture_coords_;										
out vec4 color;													
uniform sampler2D image;										
void main()														
{																
	color = vec4(a_color_) * texture(image, a_texture_coords_);  
};
