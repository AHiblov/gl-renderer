#version 330

uniform mat4 view;
uniform sampler2D colourTexture;
uniform vec3 lightDirection;
uniform vec3 diffuseLightColour;
uniform vec3 ambientLightColour;

in vec3 passCameraDirectionCam;
in vec2 passUv;
in vec3 passNormal;

out vec4 colour;

void main()
{
	vec4 textureDiffuseColour = texture2D(colourTexture, passUv);
	vec4 textureAmbientColour = textureDiffuseColour * vec4(0.25, 0.25, 0.25, 1.0);
	vec4 specularColour = vec4(0.7, 0.7, 0.7, 1.0);
	
	vec3 lightDirectionCam = (view * vec4(-lightDirection, 0.0)).xyz; //From point to light
	vec3 lightDirectionToPointCam = (view * vec4(lightDirection, 0.0)).xyz; //From light to point
	
	vec3 reflectedLight = reflect(lightDirectionToPointCam, passNormal);
	float cosAlpha = clamp(dot(passCameraDirectionCam, reflectedLight), 0.0, 1.0);
	
	colour = textureAmbientColour * vec4(ambientLightColour, 1.0) +
		textureDiffuseColour * clamp(dot(passNormal, lightDirectionCam), 0.0, 1.0) * vec4(diffuseLightColour, 1.0) +
		specularColour * pow(cosAlpha, 3.0) * vec4(diffuseLightColour, 1.0);
}