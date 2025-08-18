#version 330

in vec2 passUv;

out vec3 colour;

uniform sampler2D renderedTexture;

void main()
{
	vec3 fragmentColour = texture(renderedTexture, passUv).rgb;
	float gradient = dot(fragmentColour, vec3(0.299, 0.587, 0.114));
	colour = vec3(gradient);
}