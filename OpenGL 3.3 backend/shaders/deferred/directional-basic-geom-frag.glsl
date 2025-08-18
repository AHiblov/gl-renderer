#version 330

uniform sampler2D colourTexture;

in vec3 passPosition;
in vec2 passUv;
in vec3 passNormal;

layout(location = 0) out vec3 positionWld;
layout(location = 1) out vec3 uv;
layout(location = 2) out vec3 normal;
layout(location = 3) out vec3 diffuse;

void main()
{
	positionWld = passPosition;
	uv = vec3(passUv, 0.0);
	normal = passNormal;
	
	diffuse = texture2D(colourTexture, passUv).rgb;
}