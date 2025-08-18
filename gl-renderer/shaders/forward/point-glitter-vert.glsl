#version 450

layout(location = 0) in vec3 positionMdl;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 normalMdl;
layout(location = 3) in vec3 tangentMdl;
layout(location = 4) in vec3 bitangentMdl;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 lightPositionWld;

out vec3 passPositionWld;
out vec2 passUv;
out vec3 passNormalWld;
out vec3 passTangentWld;
out vec3 passBitangentWld;

out vec3 passLightDirectionCam;

void main()
{
	passPositionWld = (model * vec4(positionMdl, 1.0)).xyz;
	passUv = uv;
	passNormalWld = normalize((model * vec4(normalMdl, 0.0)).xyz);
	passTangentWld = normalize((model * vec4(tangentMdl, 0.0)).xyz);
	passBitangentWld = normalize((model * vec4(bitangentMdl, 0.0)).xyz);
	
	vec4 positionCam = view * model * vec4(positionMdl, 1.0);
	
	//Vector that goes from the vertex to the camera, in camera space
	vec3 vertexCam = positionCam.xyz;
	vec3 eyeDirectionCam = vec3(0.0, 0.0, 0.0) - vertexCam;
	
	//Vector that goes from the vertex to the light, in camera space. Model matrix is identity
	vec3 lightPositionCam = (view * vec4(lightPositionWld, 1.0)).xyz;
	passLightDirectionCam = normalize(lightPositionCam + eyeDirectionCam); //Halfway vector

	gl_Position = projection * positionCam;
}
