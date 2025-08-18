#version 450

uniform sampler2D normalComponent;
uniform sampler2D diffuseComponent;

uniform vec3 lightDirection;
uniform vec3 diffuseLightColour;
uniform vec3 ambientLightColour;

in vec2 passUv;

out vec3 colour;

void main()
{
	vec3 normal = texture(normalComponent, passUv).rgb;
	
	vec3 textureDiffuseColour = texture(diffuseComponent, passUv).rgb;
	vec3 textureAmbientColour = textureDiffuseColour * vec3(0.25, 0.25, 0.25);

	colour = textureAmbientColour * ambientLightColour + textureDiffuseColour * clamp(dot(normal, -lightDirection), 0.0, 1.0) * diffuseLightColour;
}