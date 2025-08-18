#version 450

uniform sampler2D colourTexture;
uniform sampler2D normalTexture;

in vec3 passPosition;
in vec2 passUv;
in mat3 passTangentToWorld;

layout(location = 0) out vec3 positionWld;
layout(location = 1) out vec3 uv;
layout(location = 2) out vec3 normal;
layout(location = 3) out vec3 diffuse;

void main()
{
	positionWld = passPosition;
	uv = vec3(passUv, 0.0);
	normal = passTangentToWorld * (texture(normalTexture, passUv).rgb * 2.0 - 1.0);
	diffuse = texture(colourTexture, passUv).rgb;
}