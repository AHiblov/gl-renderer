#version 450

layout(location = 0) in vec3 positionMdl;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 normalMdl;
layout(location = 3) in vec3 instanceOffsetMdl;
layout(location = 4) in float instanceRotationMdl;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat3 rotation; //Not used

out vec2 passUv;
out vec3 passNormal;
out vec4 passVertexPositionCam;

void main()
{
	float c = cos(instanceRotationMdl);
	float s = sin(instanceRotationMdl);
	mat3 particleRotation = mat3(
		vec3(c, 0.0, s),
		vec3(0.0, 1.0, 0.0),
		vec3(-s, 0.0, c));
	
	passUv = uv;
	passNormal = normalize(particleRotation * normalMdl);
	
	vec4 vertexPositionCam = view * model * vec4(instanceOffsetMdl + positionMdl, 1.0);
	passVertexPositionCam = vertexPositionCam;

	gl_Position = projection * view * model * vec4(instanceOffsetMdl + particleRotation * positionMdl, 1.0);
}