#version 330

layout(location = 0) in vec3 positionMdl;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 normalMdl;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat3 rotation;

out vec2 passUv;
out vec3 passNormal;
out vec4 passVertexPositionCam;

void main()
{
	passUv = uv;
	passNormal = normalize(rotation * normalMdl);
	
	vec4 vertexPositionCam = view * model * vec4(positionMdl, 1.0);
	passVertexPositionCam = vertexPositionCam;
	
	gl_Position = projection * vertexPositionCam;
}