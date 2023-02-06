#version 430 core												

uniform sampler2D image;										

in vec2 _uv;

in vec4 _color;

out vec4 color;													

void main()														
{			

	vec4 textureColor = texelFetch(image, ivec2(_uv), 0);

	if(textureColor.a == 0.0)
	{
		textureColor.r = 1.0;
		//discard;
	}

	color = _color * textureColor;  
};
