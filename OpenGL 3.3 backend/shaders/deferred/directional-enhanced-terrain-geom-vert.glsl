#version 330

layout(location = 0) in vec3 positionMdl;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 normalMdl;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat3 rotation;

out vec3 passPosition;
out vec2 passUv;
out vec3 passNormal;

void main()
{
	passPosition = (model * vec4(positionMdl, 1.0)).xyz;
	passUv = uv;
	passNormal = normalize(rotation * normalMdl);
	
	gl_Position = projection * view * model * vec4(positionMdl, 1.0);
}