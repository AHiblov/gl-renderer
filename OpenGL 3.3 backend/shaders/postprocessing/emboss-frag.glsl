#version 330

in vec2 passUv;

out vec3 colour;

uniform sampler2D renderedTexture;
uniform vec2 screenWidthHeight;

const vec3 embossKernel[3] = vec3[3](
	vec3(-1.0, -1.0, -1.0),
	vec3(-1.0, 0.0, 1.0),
	vec3(1.0, 1.0, 1.0)); //Positive and negative values are flipped so that objects are "pressed" above their background

void main()
{
	float xStep = 1.0 / screenWidthHeight.x;
	float yStep = 1.0 / screenWidthHeight.y;
	
	vec4 sumUpColour;
	sumUpColour = texture(renderedTexture, passUv + vec2(-xStep, -yStep)) * embossKernel[0][0] + texture(renderedTexture, passUv + vec2(0.0, -yStep)) * embossKernel[0][1] + texture(renderedTexture, passUv + vec2(xStep, -yStep)) * embossKernel[0][2];
	sumUpColour += texture(renderedTexture, passUv + vec2(-xStep, 0.0)) * embossKernel[1][0] + texture(renderedTexture, passUv) * embossKernel[1][1] + texture(renderedTexture, passUv + vec2(xStep, 0.0)) * embossKernel[1][2];
	sumUpColour += texture(renderedTexture, passUv + vec2(-xStep, yStep)) * embossKernel[2][0] + texture(renderedTexture, passUv + vec2(0.0, yStep)) * embossKernel[2][1] + texture(renderedTexture, passUv + vec2(xStep, yStep)) * embossKernel[2][2];
	sumUpColour /= 8.0;
	sumUpColour += 0.5;

	float lumene = dot(sumUpColour.rgb, vec3(0.299, 0.587, 0.114));
	colour = vec3(lumene);
}