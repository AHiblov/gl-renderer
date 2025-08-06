#version 450

uniform sampler2D colourTexture;
uniform vec3 lightPositionWld;
uniform float fogDensity;
uniform vec4 fogColour;

in vec2 passUv;
in vec3 passPositionWld;
in vec3 passLightDirectionCam;
in vec3 passNormalCam;
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
	float lightPower = 50.0;

	vec4 diffuseColor = texture(colourTexture, passUv);
	vec4 ambientColor = diffuseColor * vec4(0.25, 0.25, 0.25, 1.0);

	float distanceToLight = length(lightPositionWld - passPositionWld);

	float cosTheta = clamp(dot(passNormalCam, passLightDirectionCam), 0.0, 1.0);

	vec4 resultColour = vec4(ambientColor + 
		diffuseColor * lightPower * cosTheta / (distanceToLight * distanceToLight));
	colour = mix(resultColour, fogColour, calculateFogFactor());
}