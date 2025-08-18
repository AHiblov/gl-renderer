#version 450

uniform sampler2D colourTexture;
uniform vec3 lightPositionWld;
uniform float time;

in vec2 passUv;
in vec3 passPositionWld;
in vec3 passLightDirectionCam;
in vec3 passEyeDirectionCam;

const float TWO_PI = 3.14159 * 2.0;

//Values: https://www.jayconrod.com/posts/50/water-simulation-demo-in-webgl
const int waveAmount = 4;
const float waveLength[4] = float[4](1.0, 1.5, 1.2, 0.8);
const float waveSpeed[4] = float[4](1.0, 0.7, 0.8, 1.2);
const vec2 waveDirection[4] = vec2[4](vec2(-0.98481, 0.17365), vec2(-0.34202, 0.93969), vec2(0.17365, 0.98481), vec2(0.5, 0.86603)); //170, 110, 80, 60
const float amplitude[4] = float[4](0.01, 0.02, 0.015, 0.06);

out vec4 colour;

//https://www.jayconrod.com/posts/34/water-simulation-in-glsl

float dWaveDx(int i, float x, float y)
{
	float frequency = TWO_PI / waveLength[i];
	float phase = waveSpeed[i] * frequency;
	float theta = dot(waveDirection[i], vec2(x, y));
	float ampl = amplitude[i] * waveDirection[i].x * frequency;
	return ampl * cos(theta * frequency + time * phase);
}

float dWaveDy(int i, float x, float y)
{
	float frequency = TWO_PI / waveLength[i];
	float phase = waveSpeed[i] * frequency;
	float theta = dot(waveDirection[i], vec2(x, y));
	float ampl = amplitude[i] * waveDirection[i].y * frequency;
	return ampl * cos(theta * frequency + time * phase);
}

vec3 calculateWaveNormal(float x, float y)
{
	float dx = 0.0;
	float dy = 0.0;
	
	for(int i = 0; i < waveAmount; i++)
	{
		dx += dWaveDx(i, x, y);
		dy += dWaveDy(i, x, y);
	}
	
	vec3 n = vec3(-dx, -dy, 1.0);
	return normalize(n);
}

void main()
{
	float lightPower = 50.0;
	
	vec4 diffuseColor = texture(colourTexture, passUv);
	vec4 ambientColor = diffuseColor * vec4(0.25, 0.25, 0.25, 1.0);
	vec4 specularColor = vec4(0.7, 0.7, 0.7, 1.0);
	
	float distanceToLightSquare = length(lightPositionWld - passPositionWld);
	distanceToLightSquare *= distanceToLightSquare;
	
	vec3 normal = calculateWaveNormal(passPositionWld.x, passPositionWld.z);
	float specularCoefficient = max(0.0, sign(normal.y));
	
	float cosTheta = clamp(dot(normal, passLightDirectionCam), 0.0, 1.0);
	
	vec3 normalizedEyeDirectionCam = normalize(passEyeDirectionCam);
	vec3 reflectedLight = reflect(-passLightDirectionCam, normal);
	float cosAlpha = clamp(dot(normalizedEyeDirectionCam, reflectedLight), 0.0, 1.0);
	
	colour = vec4(ambientColor + 
		diffuseColor * lightPower * cosTheta / distanceToLightSquare +
		specularColor * specularCoefficient * lightPower * pow(cosAlpha, 3.0) / distanceToLightSquare);
}