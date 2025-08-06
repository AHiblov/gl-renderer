#version 450

uniform sampler2D positionComponent;
uniform sampler2D normalComponent;
uniform sampler2D diffuseComponent;

uniform vec3 lightPositionWld;
uniform vec3 diffuseLightColour;
uniform vec3 ambientLightColour;

uniform vec2 screenSize;
uniform vec2 lightParameters; //X - light power, Y - sphere radius

out vec3 colour;

void main()
{
	vec2 uv = gl_FragCoord.xy / screenSize;
	
	vec3 fragmentPosition = texture(positionComponent, uv).rgb;
	vec3 fragmentNormal = normalize(texture(normalComponent, uv).rgb);
	vec3 fragmentDiffuseColour = texture(diffuseComponent, uv).rgb;
	
	vec3 lightDirection = fragmentPosition - lightPositionWld;
	float distanceToLight = length(lightDirection);
	lightDirection = normalize(lightDirection);
	
	float cosTheta = clamp(dot(fragmentNormal, -lightDirection), 0.0, 1.0);

	colour = fragmentDiffuseColour * lightParameters.x * cosTheta / (distanceToLight * distanceToLight);
}