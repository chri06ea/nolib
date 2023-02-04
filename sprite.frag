#version 430 core												

uniform sampler2D image;										

out vec4 color;													

void main()														
{																
	color = vec4(1, 0,0,1); // * texture(image, a_texture_coords_);  
};
