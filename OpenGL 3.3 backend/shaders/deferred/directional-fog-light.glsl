#version 330

uniform sampler2D positionComponent;
uniform sampler2D normalComponent;
uniform sampler2D diffuseComponent;

uniform vec3 lightDirection;
uniform vec3 diffuseLightColour;
uniform vec3 ambientLightColour;

uniform mat4 view;
uniform float fogDensity;
uniform vec4 fogColour;

in vec2 passUv;

out vec3 colour;

float calculateFogFactor(float fragmentDepth)
{
	float factor = exp(-fogDensity * fragmentDepth);
	return 1.0 - clamp(factor, 0.0, 1.0);
}

void main()
{
	vec3 normal = texture(normalComponent, passUv).rgb;
	
	vec3 textureDiffuseColour = texture(diffuseComponent, passUv).rgb;
	vec3 textureAmbientColour = textureDiffuseColour * vec3(0.25, 0.25, 0.25);

	vec3 resultColour  = textureAmbientColour * ambientLightColour + textureDiffuseColour * clamp(dot(normal, -lightDirection), 0.0, 1.0) * diffuseLightColour;
	vec4 vertexPosition = view * texture(positionComponent, passUv);

	colour = mix(resultColour, fogColour.rgb, calculateFogFactor(abs(vertexPosition.z)));
}