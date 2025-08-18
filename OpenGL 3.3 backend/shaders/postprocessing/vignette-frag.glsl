#version 330

in vec2 passUv;

out vec3 colour;

uniform sampler2D renderedTexture;
uniform float screenRatio;

void main()
{
	vec3 fragmentColour = texture(renderedTexture, passUv).rgb;
	
	vec2 transformedUv = passUv - 0.5; // [-0.5, 0.5]
	transformedUv.x *= screenRatio;
	float dist = length(transformedUv);
	float vignette = smoothstep(0.7, 1.0, dist);
	
	colour = mix(fragmentColour, vec3(0.0, 0.0, 0.0), vignette);
}