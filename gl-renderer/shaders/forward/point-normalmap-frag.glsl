#version 450

uniform sampler2D colourTexture;
uniform sampler2D normalTexture;
uniform vec3 lightPositionWld;

in vec2 passUv;
in vec3 passPositionWld;
in vec3 passLightDirectionTang;

out vec4 colour;

void main()
{
	float lightPower = 50.0;
	
	vec4 diffuseColor = texture(colourTexture, passUv);
	vec4 ambientColor = diffuseColor * vec4(0.25, 0.25, 0.25, 1.0);
	
	vec4 textureNormalTang = normalize(texture(normalTexture, passUv) * 2.0 - 1.0);
	
	float distanceToLight = length(lightPositionWld - passPositionWld);
	
	float cosTheta = clamp(dot(textureNormalTang.xyz, passLightDirectionTang), 0.0, 1.0);
	
	colour = vec4(ambientColor + 
		diffuseColor * lightPower * cosTheta / (distanceToLight * distanceToLight));
}