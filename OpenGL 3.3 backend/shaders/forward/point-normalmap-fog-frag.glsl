#version 330

uniform sampler2D colourTexture;
uniform sampler2D normalTexture;
uniform vec3 lightPositionWld;
uniform float fogDensity;
uniform vec4 fogColour;

in vec2 passUv;
in vec3 passPositionWld;
in vec3 passLightDirectionTang;
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

	vec4 diffuseColor = texture2D(colourTexture, passUv);
	vec4 ambientColor = diffuseColor * vec4(0.25, 0.25, 0.25, 1.0);

	vec4 textureNormalTang = normalize(texture2D(normalTexture, passUv) * 2.0 - 1.0);

	float distanceToLight = length(lightPositionWld - passPositionWld);

	float cosTheta = clamp(dot(textureNormalTang.xyz, passLightDirectionTang), 0.0, 1.0);

	vec4 resultColour = vec4(ambientColor + 
		diffuseColor * lightPower * cosTheta / (distanceToLight * distanceToLight));
	colour = mix(resultColour, fogColour, calculateFogFactor());
}