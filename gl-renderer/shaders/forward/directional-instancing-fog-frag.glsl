#version 450

uniform sampler2D colourTexture;
uniform vec3 lightDirection;
uniform vec3 diffuseLightColour;
uniform vec3 ambientLightColour;
uniform float fogDensity;
uniform vec4 fogColour;

in vec2 passUv;
in vec3 passNormal;
in vec4 passVertexPositionCam;

out vec4 colour;

float calculateFogFactor()
{
	float fragmentDepth = abs(passVertexPositionCam.z / passVertexPositionCam.w);
	
	float factor = exp(-fogDensity * fragmentDepth);
	return 1.0 - clamp(factor, 0.0, 1.0);
}

void main()
{
	vec4 textureDiffuseColour = texture(colourTexture, passUv);
	vec4 textureAmbientColour = textureDiffuseColour * vec4(0.25, 0.25, 0.25, 1.0);

	vec4 resultColour = textureAmbientColour * vec4(ambientLightColour, 1.0) + textureDiffuseColour * clamp(dot(passNormal, -lightDirection), 0.0, 1.0) * vec4(diffuseLightColour, 1.0);
	colour = mix(resultColour, fogColour, calculateFogFactor());
}