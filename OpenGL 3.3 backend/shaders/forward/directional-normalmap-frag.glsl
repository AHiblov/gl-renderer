#version 330

uniform sampler2D colourTexture;
uniform sampler2D normalTexture;

uniform vec3 diffuseLightColour;
uniform vec3 ambientLightColour;

in vec2 passUv;
in vec3 passLightDirectionTang;

out vec4 colour;

void main()
{
	vec4 textureDiffuseColour = texture2D(colourTexture, passUv);
	vec4 textureAmbientColour = textureDiffuseColour * vec4(0.25, 0.25, 0.25, 1.0);
	
	vec4 textureNormalTang = normalize(texture2D(normalTexture, passUv) * 2.0 - 1.0);
	
	colour = textureAmbientColour * vec4(ambientLightColour, 1.0) + textureDiffuseColour * clamp(dot(textureNormalTang.xyz, -passLightDirectionTang), 0.0, 1.0) * vec4(diffuseLightColour, 1.0);
}