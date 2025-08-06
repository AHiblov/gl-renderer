#version 450

layout(location = 0) in vec3 positionMdl;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 normalMdl;
layout(location = 3) in vec3 tangentMdl;
layout(location = 4) in vec3 bitangentMdl;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat3 rotation;

uniform vec3 lightDirection;

out vec3 passPosition;
out vec2 passUv;
out mat3 passTangentToWorld;

void main()
{
	passPosition = (model * vec4(positionMdl, 1.0)).xyz;
	passUv = uv;
	
	vec3 rotatedNormal = normalize(rotation * normalMdl);
	
	mat3 model3 = mat3(model); //For deferred shading calculate in world space since weconvert from tangent to world
	
	vec3 tangentWld = model3 * tangentMdl;
	vec3 bitangentWld = model3 * bitangentMdl;
	vec3 normalWld = model3 * normalMdl; //Normal must have the same rotation as normalmap
	
	passTangentToWorld = mat3(
		tangentWld.x, tangentWld.y, tangentWld.z,
		bitangentWld.x, bitangentWld.y, bitangentWld.z,
		normalWld.x, normalWld.y, normalWld.z);
	
	gl_Position = projection * view * model * vec4(positionMdl, 1.0);
}