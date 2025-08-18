#version 330

layout(location = 0) in vec3 positionMdl;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 normalMdl;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat3 rotation;

out vec3 passCameraDirectionCam;
out vec2 passUv;
out vec3 passNormal;

void main()
{
	vec4 positionCam = view * model * vec4(positionMdl, 1.0);
	
	passCameraDirectionCam = normalize(vec3(0.0, 0.0, 0.0) - positionCam.xyz);
	
	passUv = uv;
	passNormal = normalize((view * model * vec4(normalMdl, 0.0)).xyz);
	
	gl_Position = projection * positionCam;
}