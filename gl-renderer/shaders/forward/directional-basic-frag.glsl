#version 450

uniform sampler2D colourTexture;
uniform vec3 lightDirection;
uniform vec3 diffuseLightColour;
uniform vec3 ambientLightColour;

in vec2 passUv;
in vec3 passNormal;

out vec4 colour;

void main()
{
	vec4 textureDiffuseColour = texture(colourTexture, passUv);
	vec4 textureAmbientColour = textureDiffuseColour * vec4(0.25, 0.25, 0.25, 1.0);
	
	colour = textureAmbientColour * vec4(ambientLightColour, 1.0) + textureDiffuseColour * clamp(dot(passNormal, -lightDirection), 0.0, 1.0) * vec4(diffuseLightColour, 1.0);
}