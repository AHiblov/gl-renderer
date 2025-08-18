#version 450

layout(location = 0) in vec3 positionMdl;

out vec2 passUv;

void main()
{
	gl_Position = vec4(positionMdl, 1.0);
	passUv = (positionMdl.xy + vec2(1.0, 1.0)) / 2.0;
}