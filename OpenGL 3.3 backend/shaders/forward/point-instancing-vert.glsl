#version 330

layout(location = 0) in vec3 positionMdl;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 normalMdl;
layout(location = 3) in vec3 instanceOffsetMdl;
layout(location = 4) in float instanceRotationMdl;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 lightPositionWld;

out vec2 passUv;
out vec3 passPositionWld;
out vec3 passLightDirectionCam;
out vec3 passNormalCam;

void main()
{
	float c = cos(instanceRotationMdl);
	float s = sin(instanceRotationMdl);
	mat3 particleRotation = mat3(
		vec3(c, 0.0, s),
		vec3(0.0, 1.0, 0.0),
		vec3(-s, 0.0, c));

	vec4 positionCam = view * model * vec4(instanceOffsetMdl + particleRotation * positionMdl, 1.0);

	gl_Position = projection * positionCam;
   
	passUv = uv;
   
	passPositionWld = (model * vec4(positionMdl, 1.0)).xyz;
   
	//Vector that goes from the vertex to the camera, in camera space
	vec3 vertexCam = positionCam.xyz;
	vec3 eyeDirectionCam = vec3(0.0, 0.0, 0.0) - vertexCam;
   
	//Vector that goes from the vertex to the light, in camera space. Model matrix is identity
	vec3 lightPositionCam = (view * vec4(lightPositionWld, 1.0)).xyz;
	passLightDirectionCam = normalize(lightPositionCam + eyeDirectionCam); //Halfway vector
   
	passNormalCam = normalize((view * model * vec4(particleRotation * normalMdl, 0.0)).xyz);
}
