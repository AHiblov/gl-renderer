#version 450

layout(location = 0) in vec3 positionModelspace;
layout(location = 1) in vec2 uv;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 passUv;

void main()
{
	passUv = uv;				
	gl_Position = (projection * view * model * vec4(positionModelspace, 1.0)).xyww;
}
