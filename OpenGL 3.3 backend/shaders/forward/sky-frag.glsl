#version 330

uniform sampler2D colourTexture;

in vec2 passUv;

out vec4 colour;

void main()
{
	colour = texture2D(colourTexture, passUv);
}
