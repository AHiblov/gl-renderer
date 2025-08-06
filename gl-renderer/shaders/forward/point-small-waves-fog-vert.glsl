#version 450

layout(location = 0) in vec3 positionMdl;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 normalMdl;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 lightPositionWld;

out vec2 passUv;
out vec3 passPositionWld;
out vec3 passLightDirectionCam;
out vec4 passVertexPositionCam;

void main()
{
	vec4 positionCam = view * model * vec4(positionMdl, 1.0);

	gl_Position = projection * positionCam;
   
	passUv = uv;
   
	passPositionWld = (model * vec4(positionMdl, 1.0)).xyz;
   
	//Vector that goes from the vertex to the camera, in camera space
	vec3 vertexCam = positionCam.xyz;
	vec3 eyeDirectionCam = vec3(0.0, 0.0, 0.0) - vertexCam;
   
	//Vector that goes from the vertex to the light, in camera space
	vec3 lightPositionCam = (view * vec4(lightPositionWld, 1.0)).xyz;
	passLightDirectionCam = normalize(lightPositionCam + eyeDirectionCam); //Halfway vector
	
	passVertexPositionCam = positionCam;
}
