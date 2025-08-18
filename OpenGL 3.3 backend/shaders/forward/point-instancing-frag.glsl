#version 330

uniform sampler2D colourTexture;
uniform vec3 lightPositionWld;

in vec2 passUv;
in vec3 passPositionWld;
in vec3 passLightDirectionCam;
in vec3 passNormalCam;

out vec4 colour;

void main()
{
	float lightPower = 50.0;
	
	vec4 diffuseColor = texture2D(colourTexture, passUv);
	vec4 ambientColor = diffuseColor * vec4(0.25, 0.25, 0.25, 1.0);
	
	float distanceToLight = length(lightPositionWld - passPositionWld);
	
	float cosTheta = clamp(dot(passNormalCam, passLightDirectionCam), 0.0, 1.0);
	
	colour = vec4(ambientColor + 
		diffuseColor * lightPower * cosTheta / (distanceToLight * distanceToLight));
}