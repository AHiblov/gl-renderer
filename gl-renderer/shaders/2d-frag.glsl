#version 450

uniform sampler2D colourTexture;

in vec2 passUv;

layout(location = 0) out vec4 colour;

void main()
{
	colour = texture(colourTexture, passUv);
}