#version 430 core												

uniform sampler2D image;										

in vec2 _tex_coords;

out vec4 color;													

void main()														
{			

  vec4 textureColor = texelFetch(image, ivec2(_tex_coords), 0);

	if(textureColor.a == 0.0)
		discard;

	color = vec4(1, 1, 1, 1) * textureColor;  
};
