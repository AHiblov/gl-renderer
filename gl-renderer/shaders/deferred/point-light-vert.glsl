#version 450

layout(location = 0) in vec3 positionMdl;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec2 lightParameters; //X - light power, Y - sphere radius

void main()
{
	mat4 scale = mat4(lightParameters.y);
	scale[3][3] = 1.;

	gl_Position = projection * view * model * scale * vec4(positionMdl, 1.);
}