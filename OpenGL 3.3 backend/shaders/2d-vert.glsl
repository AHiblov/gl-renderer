#version 330

layout(location = 0) in vec2 vertexPositionModelspace;
layout(location = 1) in vec2 vertexUv;

uniform float screenRatio;

out vec2 passUv;

void main()
{
	passUv = vertexUv;
	gl_Position = vec4(vec2(vertexPositionModelspace.x / screenRatio, vertexPositionModelspace.y), 0.0, 1.0);
}