#version 450

uniform sampler2D colourTexture;
uniform vec3 lightPositionWld;

in vec2 passUv;
in vec3 passPositionWld;
in vec3 passLightDirectionCam;
in vec3 passNormalCam;
in vec3 passEyeDirectionCam;

out vec4 colour;

void main()
{
	float lightPower = 50.0;
	
	vec4 diffuseColor = texture(colourTexture, passUv);
	vec4 ambientColor = diffuseColor * vec4(0.25, 0.25, 0.25, 1.0);
	vec4 specularColor = vec4(0.7, 0.7, 0.7, 1.0);
	
	float distanceToLightSquare = length(lightPositionWld - passPositionWld);
	distanceToLightSquare *= distanceToLightSquare;
	
	float cosTheta = clamp(dot(passNormalCam, passLightDirectionCam), 0.0, 1.0);
	
	vec3 normalizedEyeDirectionCam = normalize(passEyeDirectionCam);
	vec3 reflectedLight = reflect(-passLightDirectionCam, passNormalCam);
	float cosAlpha = clamp(dot(normalizedEyeDirectionCam, reflectedLight), 0.0, 1.0);
	
	colour = vec4(ambientColor + 
		diffuseColor * lightPower * cosTheta / distanceToLightSquare +
		specularColor * lightPower * pow(cosAlpha, 3.0) / distanceToLightSquare);
}